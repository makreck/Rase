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

bool EspTool::open_interface(const char* _ifac, int& _fd) {
    if (_fd > -1) {
        close(_fd);
    }

    _fd = EspTool::open_serial_port(_ifac, B115200);

    if (_fd < 0) {
        return (false);
    }

    // Read possible dirt from the ESP32 Debug Output to avoid interference with commanding data!
    uint8_t dummy_in[4096];
    read(_fd, dummy_in, sizeof (dummy_in));

    return (true);
}

bool EspTool::close_interface(int& _fd) {
    if (_fd == -1) {
        return (false);
    }
    close(_fd);
    _fd = -1;
    return (true);
}

char* EspTool::allocate_command(const char* _cmd) {
    ssize_t size = strlen(_cmd) + 2;
    char* command_string = (char *)malloc(size);
    if (command_string == nullptr) {
        printf("Error, unable to alloc %d bytes for command string!\n", (int)size);
        return (nullptr);
    }
    memset(command_string, 0, size);
    strncpy(command_string, _cmd, size - 1);
    return (command_string);
}

char* EspTool::transact_command(const char* _ifac, const char* _cmd) {
    if (_cmd == nullptr) {
        printf("Error, tranaction not prepared!");
        return (nullptr);
    }

    int fd = -1;
    if (!EspTool::open_interface(_ifac, fd)) {
        return (nullptr);
    }

    ssize_t size_out = strlen(_cmd);
    ssize_t len_out = write(fd, _cmd, size_out);
    if (size_out != len_out) {
        printf("Error, unable to transmit %d bytes command string, %d bytes written!\n", (int)size_out, (int)len_out);
        return (nullptr);
    }

    usleep(500000);
    
    ssize_t size = 65536;
    char* in = (char*)malloc(size);
    if (in == nullptr) {
        printf("Error, unable to allocate response data buffer!\n");
        return (nullptr);
    }
    memset(in, 0, size);

    ssize_t len_read = 0;
    ssize_t length = 0;
    do {
        len_read = read(fd, &in[length], size - length - 1);
        if (len_read > 0) {
            length += len_read;
            usleep(25000);
        }
    } while (len_read > 0);
    in[length] = '\0';

    if (length == 0) {
        free(in);
        return (nullptr);
    }

    char* response = in;
    char* p = strstr(in, "<!DOCTYPE html>");
    if (p != nullptr) {
        response = p;
        char* p = strstr(in, "</html>");
        if (p != nullptr) {
            p[7] = '\0';
        }
        length = strlen(response);
#ifndef DISABLE_FILTER
    } else {
        int level = -1;
        int i;
        for (i = 0; (i < length) && (level != 0); i++) {
            if (in[i] == '{') {
                if (level == -1) {
                    level = 1;
                    response = &in[i];
                } else {
                    level++;
                }
            }
            if (in[i] == '}') {
                level--;                
            }
        }
        in[i++] = '\r';
        in[i++] = '\n';
        in[i] = '\0';
        length = i;
#endif        
    }

    char* result = (char *)malloc(length + 1);
    if (result == nullptr) {
        free(in);
        printf("Unable to allocate %d bytes for result data!\n", (int)(length + 1));
        return (nullptr);
    }
    memset(result, 0, length + 1);
    strncpy(result, response, length);
    free(in);

    EspTool::close_interface(fd);

    return (result);
}

bool EspTool::read_config(const char* _ifac, DevConfig* _dev) {
    if (_dev != nullptr) {
        char* config_json = EspTool::transact_command(_ifac, TTY_KEY_API_CONFIG_GET);
        if (config_json != nullptr) {
            _dev->parse_config_json(config_json);
            free(config_json);
            return (true);
        }
    }
    return (false);
}

bool EspTool::read_id(const char* _ifac, DevConfig* _dev) {
    if (_dev != nullptr) {
        char* id_json = EspTool::transact_command(_ifac, TTY_KEY_API_IDENTIFY);
        if (id_json != nullptr) {
            _dev->parse_id_json(id_json);
            free(id_json);
            return (true);
        }
    }
    return (false);
}

bool EspTool::read_data(const char* _ifac, DevConfig* _dev) {
    if (_dev != nullptr) {
        if (EspTool::read_id(_ifac, _dev)) {
            if (EspTool::read_config(_ifac, _dev)) {
                return (true);
            }
        }
    }
    return (false);
}

bool EspTool::find_tty_devices(std::vector<DevConfig*>* _device_list, pthread_mutex_t* _device_list_mutex) {
    if (_device_list == nullptr) {
        return (false);
    }

    pthread_t threads[MAX_TTY * 2]{ 0 };
    int count = 0;

    for (int tty = 0; tty < MAX_TTY; tty++) {
        char ifac[2][16]{ 0 };
        snprintf(ifac[0], sizeof (ifac[0]) - 1, "/dev/ttyACM%d", tty);
        snprintf(ifac[1], sizeof (ifac[1]) - 1, "/dev/ttyUSB%d", tty);

        for (int i = 0; i < 2; i++) {
            int fd = EspTool::open_serial_port(ifac[i], B115200);
            if (fd >= 0) {
                close(fd);
                DevConfig* device = new DevConfig();
                device->set_tty_interface(ifac[i]);
                pthread_t thread_handle = 0;
                if (pthread_create(&thread_handle, nullptr, EspTool::_scanner_thread, new ScanTTY(device, _device_list, _device_list_mutex)) == 0) {
                    threads[count++] = thread_handle;
                }
            }
        }
    }

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            pthread_join(threads[i], nullptr);
        }
    }

    return (true);
}

void* EspTool::_scanner_thread(void* _object) {
    ScanTTY* scan = (ScanTTY*)_object;

    if (EspTool::read_data(scan->device->tty_ifac, scan->device)) {
        pthread_mutex_lock(scan->device_list_mutex); {
            if (!scan->device->register_device(scan->device_list)) {
                delete (scan->device);
            }
        } pthread_mutex_unlock(scan->device_list_mutex);
    } else {
        delete (scan->device);
    }

    delete (scan);
    return (nullptr);
}

const char* EspTool::find_matching_firmware_image(const char* _chip_type) {
    if (_chip_type == nullptr) {
        return (nullptr);
    }

    const char* firmware_file = nullptr;
    if (strstr(_chip_type, FIRMWARE_CHIP_KEY_ESP32S3_DEVKIT_1) != nullptr) {
        firmware_file = "./firmware_images/image_esp32-s3-devkitc-1.bin";
    } else if (strstr(_chip_type, FIRMWARE_CHIP_KEY_SEEED_STUDIO_XIAO_ESP32S3) != nullptr) {
        return firmware_file = "./firmware_images/image_seeed_xiao_esp32s3.bin";
    } else if (strstr(_chip_type, FIRMWARE_CHIP_KEY_WAVESHARE_ESP32S3_MINI) != nullptr) {
        firmware_file = "./firmware_images/image_waveshare_esp32s3_mini.bin";
    } else if (strstr(_chip_type, FIRMWARE_CHIP_KEY_DIYMORE_ESP32S3_SUPER_MINI) != nullptr) {
        firmware_file = "./firmware_images/image_alks_esp32s3_mini.bin";
    }

    // ****
    printf("---> Firmware matching for chip type \"%s\" is \"%s\"\n",
        _chip_type, (firmware_file != nullptr) ? firmware_file : "---");

    return (firmware_file);
}


pthread_t EspTool::update_all_devices(std::vector<DevConfig*>& _device_list, OTALoaderCB _callback, void* _user_param) {
    pthread_t thread_handle = 0;
    pthread_create(&thread_handle, nullptr, EspTool::_ota_control_thread, new MultiCmdParms(&_device_list, _callback, _user_param));
    return (thread_handle);
}

void* EspTool::_ota_control_thread(void* _object) {
    MultiCmdParms* parms = reinterpret_cast<MultiCmdParms*>(_object);
    
    std::vector<pthread_t> threads;
    for (int id = 0; id < parms->device_list->size(); id++) {
        DevConfig* entry = (*parms->device_list)[id];
        if (entry != nullptr) {
            if (entry->ip_ifac[0] != '\0') {
                const char* firmware_file = EspTool::find_matching_firmware_image(entry->id.chip_type);
                if (firmware_file != nullptr) {
                    threads.push_back(IPDevice::firmware_loader(id, entry->ip_ifac, firmware_file, parms->callback, parms->user_param));
                }
            }
        }
    }

    for (pthread_t &handle : threads) {
        if (handle != 0) {
            pthread_join(handle, nullptr);
            handle = 0;
        }
    }
    threads.clear();

    delete (parms);
    return (nullptr);
}

pthread_t EspTool::transact_multi_device_command(std::vector<DevConfig*>& _device_list, const char* command, OTALoaderCB _callback, void* _user_param) {
    pthread_t thread_handle = 0;
    pthread_create(&thread_handle, nullptr, EspTool::_cmd_control_thread, new MultiCmdParms(&_device_list, _callback, _user_param, command));
    return (thread_handle);
}

void* EspTool::_cmd_control_thread(void* _object) {
    MultiCmdParms* parms = reinterpret_cast<MultiCmdParms*>(_object);

    if (parms->command != nullptr) {
        std::vector<pthread_t> threads;
        for (int id = 0; id < parms->device_list->size(); id++) {
            DevConfig* entry = (*parms->device_list)[id];
            if (entry != nullptr) {
                pthread_t thread_handle = 0;
                pthread_create(&thread_handle, nullptr, EspTool::_cmd_exec_thread, new MultiCmdParms(entry, parms->callback, parms->user_param, parms->command));
            }
        }

        for (pthread_t &handle : threads) {
            if (handle != 0) {
                pthread_join(handle, nullptr);
                handle = 0;
            }
        }
        threads.clear();
    }

    delete (parms);
    return (nullptr);
}

void* EspTool::_cmd_exec_thread(void* _object) {
    MultiCmdParms* parms = reinterpret_cast<MultiCmdParms*>(_object);

    char* response = nullptr;

    if (parms->device->tty_ifac[0] != '\0') {
        response = EspTool::transact_command(parms->device->tty_ifac, parms->command);
    } else if (parms->device->ip_ifac[0] != '\0') {
        response = IPDevice::transact_http_request(parms->device->ip_ifac, parms->command, SENSOR_REQ_TYPE_JSON, NETW_RESPONSE_TIMEOUT);
    } else {
        printf("Error: No interface!\n"); // ****
    }

    if (response != nullptr) {
printf("Response:\n\"\"\"\n%s\n\"\"\"\n", response); // ****
        free(response);
    }

    delete (parms);
    return (nullptr);
}

// char* response = EspTool::transact_command(m.ifac, TTY_KEY_API_INITIALIZE);
