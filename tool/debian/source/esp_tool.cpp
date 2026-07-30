/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,      Preliminary Configuration Tool
 *  COPYRIGHT:   (C)2025-2026 KKS-Elektronik,  M. Kreck, <makreck@googlemail.com>
 *
 *  This program is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  This program is distributed in the hope that it will be useful,   but WITHOUT
 *  ANY WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE, see the GNU General Public License for details.
 *
 *  You should have received a copy of the  GNU General Public License along with
 *  this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * ==============================================================================
 */

#include "includes.h"


const PartitionEntry EspTool::partitions[6] = {
    { "nvs",      0x01, 0x00, 0x009000, 0x005000, 0 },
    { "otadata",  0x01, 0x01, 0x00e000, 0x002000, 0 },
    { "app0",     0x00, 0x00, 0x010000, 0x180000, 0 },
    { "app1",     0x00, 0x01, 0x190000, 0x180000, 0 },
    { "spiffs",   0x01, 0x02, 0x310000, 0x0E0000, 0 },
    { "coredump", 0x01, 0x03, 0x3F0000, 0x010000, 0 }
};

const char* EspTool::ota_put_req_string =   "PUT /update HTTP/1.1\r\n"
                                            "Host: %s\r\n"
                                            "Content-Length: %ld\r\n"
                                            "Connection: close\r\n"
                                            "\r\n";

bool EspTool::force_reset_over_tty(int _fd) {
    if (_fd < 0) {
        return (false);
    }


    int original = 0;
    ioctl(_fd, TIOCMGET, &original);

    int status = original;
    status &= ~TIOCM_DTR;
    ioctl(_fd, TIOCMSET, &status);
    usleep(1000);
    
    status |= TIOCM_DTR;
    ioctl(_fd, TIOCMSET, &status);
    usleep(1000);
    
    status &= ~TIOCM_RTS;
    ioctl(_fd, TIOCMSET, &status);
    usleep(1000);

    ioctl(_fd, TIOCMSET, &original);
    
    return (true);
}

bool EspTool::find_interface(char* _ifac, size_t _length) {
    char ifac[PATH_MAX]{ 0 };
    int fd = -1;
    int index = 0;
    do {
        snprintf(ifac, sizeof (ifac) - 1, "/dev/ttyACM%d", index);
        fd = EspTool::open_serial_port(ifac, B115200);
        if (fd >= 0) break;
        snprintf(ifac, sizeof (ifac) - 1, "/dev/ttyUSB%d", index);
        fd = EspTool::open_serial_port(ifac, B115200);
        if (fd >= 0) break;
    } while (++index < 10);

    if (fd >= 0) {
        close(fd);
        if (_ifac != nullptr) {
            memset(_ifac, 0, _length);
            strncpy(_ifac, ifac, _length - 1);
        }
    }

    return (fd >= 0);
}

int EspTool::open_serial_port(const char* _ifac, speed_t _baudrate) {
    if (_ifac == nullptr) return (-1);

    int fd = open(_ifac, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        return (fd);
    }

    termios terminal{ 0 };
    terminal.c_cflag = CS8 | CREAD;
    terminal.c_iflag = IGNCR | IGNBRK | IGNPAR | IXANY;
    terminal.c_oflag = 0;
    terminal.c_lflag = IEXTEN | CLOCAL | NOFLSH;
    terminal.c_cc[VTIME] = 10;
    terminal.c_cc[VMIN] = 0;

    cfsetspeed(&terminal, _baudrate);
    tcsetattr(fd, TCSANOW, &terminal);
    tcflush(fd, TCIOFLUSH);

    return (fd);
}

bool EspTool::load_binary(const char* _filename, uint8_t** _image, ssize_t* _length) {
    if (_filename == nullptr) {
        return (false);
    }

    struct stat st{ 0 };
    if (lstat(_filename, &st) == -1) {
        return (false);
    }

    int fd = open(_filename, O_RDONLY);
    if (fd < 0) {
        return (false);
    }

    ssize_t length = st.st_size;
    if (length < 1) {
        return (false);
    }

    uint8_t* data = (uint8_t*)malloc(length);
    if (data == nullptr) {
        close(fd);
        return (false);
    }

    ssize_t size = read(fd, data, length);

    close(fd);
    if (size != length) {
        free(data);
        return (false);
    }

    if (_image != nullptr) {
        *_image = data;
    } else {
        free(data);
    }

    if (_length != nullptr) {
        *_length = length;
    }

    return (true);
}

pthread_t EspTool::firmware_loader(const char* _ifac, const char* _filename, EspLoaderCB _callback, void* _user_param) {
    pthread_t thread_handle = 0;
    pthread_create(&thread_handle, nullptr, EspTool::_loader_thread, new EspToolParms(_ifac, _filename, _callback, _user_param));
    return (thread_handle);
}

void* EspTool::_loader_thread(void* _object) {
    EspToolParms* parms = (EspToolParms*)_object;

    if ((parms->image_data != nullptr) && (parms->image_length > 0)) {
        EspTool::ota_loader(parms->ifac, parms->image_data, parms->image_length, parms->callback, parms->user_param);
    }

    delete (parms);
    return (nullptr);
}


bool EspTool::ota_loader(const char* _ip_addr, uint8_t* _firmware_image, ssize_t _size, EspLoaderCB _callback, void* _user_param) {
    const char* topic = "OTA update";

    struct addrinfo hints{ 0 };
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (_callback != nullptr) {
        const char* p = "Connect...";
        (*_callback)(_user_param, topic, p);
    }

    struct addrinfo* res = nullptr; 
    int s = getaddrinfo(_ip_addr, "80", &hints, &res);
    if (s != 0) {
        return (true);
    }

    int fd = -1;
    for (struct addrinfo* rp = res; rp != nullptr; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1) {
            continue;
        }
        if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(fd);
        fd = -1;
    }

    freeaddrinfo(res);
    if (fd == -1) {
        return (false);
    }

    ssize_t req_len = snprintf(nullptr, 0, ota_put_req_string, _ip_addr, _size);
    char* request = (char*)malloc(req_len + 2);
    if (request == nullptr) {
        return (false);
    }
    snprintf(request, req_len + 1, ota_put_req_string, _ip_addr, _size);

    ssize_t sent_len = send(fd, request, req_len, 0);
    free(request);
    if (sent_len != req_len) {
        close(fd);
        return (false);
    }

    ssize_t i = 0;
    float percent = 0.0f;
    while (i < _size) {
        ssize_t chunk_len = ((_size - i) > OTA_CHUNK_SIZE) ? OTA_CHUNK_SIZE : (_size - i);
        sent_len = send(fd, &_firmware_image[i], chunk_len, 0);
        if (sent_len == -1) {
            close(fd);
            return (false);
        }
        i += sent_len;

        float progress = (float)i * 100.0f / (float)_size;
        if (fabsf(progress - percent) >= 5.0f) {
            percent = progress;
            if (_callback != nullptr) {
                char string[64]{ 0 };
                snprintf(string, sizeof (string), "%.0f%%, %zu / %zu", percent, i, _size);
                (*_callback)(_user_param, topic, string);
            }
        }
    }

    close(fd);

    if (_callback != nullptr) {
        const char* p = "Complete.";
        (*_callback)(_user_param, topic, p);
    }

    return (true);
}
