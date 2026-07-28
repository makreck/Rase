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


/*
# Name,   Type, SubType, Offset,   Size,     Flags
nvs,      data, nvs,     0x9000,   0x5000,
otadata,  data, ota,     0xe000,   0x2000,
app0,     app,  ota_0,   0x10000,  0x180000,
app1,     app,  ota_1,   0x190000, 0x180000,
spiffs,   data, spiffs,  0x310000, 0xE0000,
coredump, data, coredump,0x3F0000, 0x10000,
*/

#include "includes.h"

// void EspTool::init(const char* _ifac, EspLoaderCB _callback, void* _user_param) {
// }

// void EspTool::cleanup(void) {
// }

bool EspTool::force_reset_over_tty(int _fd) {
    if (_fd < 0) {
        return (false);
    }

    int status = 0;
    ioctl(_fd, TIOCMGET, &status);

    status &= ~TIOCM_DTR;
    ioctl(_fd, TIOCMSET, &status);
    usleep(1000);
    
    status |= TIOCM_DTR;
    ioctl(_fd, TIOCMSET, &status);
    usleep(1000);
    
    status &= ~TIOCM_RTS;
    ioctl(_fd, TIOCMSET, &status);
    
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
    terminal.c_cc[VTIME] = 5;
    terminal.c_cc[VMIN] = 1;

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

bool EspTool::upload_2nd_level(const char* _filename, const char* _ifac, EspLoaderCB _callback, void* _user_param) {
    uint8_t* data = nullptr;
    ssize_t length = 0;
    if (!load_binary(_filename, &data, &length)) {
        return (false);
    }

    int fd = open_serial_port(_ifac);
    if (fd < 0) {
        return (false);
    }





    free(data);
    close(fd);

    return (true);
}
