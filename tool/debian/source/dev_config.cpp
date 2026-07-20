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

const char* DevConfig::config_json_format =
    "\n"
    "{\n"
    "\t\"version\": \"0x%-8.8X\",\n"
    
    "\t\"ssid\": \"%s\",\n"
    "\t\"password\": \"%s\",\n"
    "\t\"wifi_channel\": \"%d\",\n"
    
    "\t\"mqtt_broker\": \"%s\",\n"
    "\t\"mqtt_username\": \"%s\",\n"
    "\t\"mqtt_password\": \"%s\",\n"
    "\t\"mqtt_enable\": \"%s\",\n"
    
    "\t\"display_layout\": \"%s\",\n"
    "\t\"display_param\": \"%d\",\n"
    "\t\"display_rotation\": \"%d\",\n"
    "\t\"display_timeout\": \"%.0f\",\n"
    "\t\"display_contrast\": \"%.0f\",\n"

    "\t\"sensor_type\": \"%s\",\n"
    "\t\"led_intensity\": \"%.0f\"\n"
    "}\n";

void DevConfig::clear(void) {
    memset(&cfg, 0, sizeof (cfg));
    memset(&id,  0, sizeof (id));
}

int DevConfig::open_port(const char* _ifac, speed_t _baudrate) {
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
    terminal.c_cc[VTIME] = 1;
    terminal.c_cc[VMIN] = 1;

    cfsetspeed(&terminal, _baudrate);
    tcsetattr(fd, TCSANOW, &terminal);
    tcflush(fd, TCIOFLUSH);

    return (fd);
}

bool DevConfig::open_interface(const char* _ifac, int& _fd) {
    if (_fd > -1) {
        close(_fd);
    }

    _fd = DevConfig::open_port(_ifac, 115200);

    if (_fd < 0) {
        return (false);
    }

    // Read possible dirt from the ESP32 Debug Output to avoid interference with commanding data!
    uint8_t dummy_in[4096];
    read(_fd, dummy_in, sizeof (dummy_in));

    return (true);
}

bool DevConfig::close_interface(int& _fd) {
    if (_fd == -1) {
        return (false);
    }
    close(_fd);
    _fd = -1;
    return (true);
}

bool DevConfig::find_interface(char* _ifac, size_t _length) {
    char ifac[PATH_MAX]{ 0 };
    int fd = -1;
    int index = 0;
    do {
        snprintf(ifac, sizeof (ifac) - 1, "/dev/ttyACM%d", index);
        fd = open_port(ifac, 115200);
        if (fd >= 0) break;
        snprintf(ifac, sizeof (ifac) - 1, "/dev/ttyUSB%d", index);
        fd = open_port(ifac, 115200);
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

size_t DevConfig::json_get(char* json_data, const char* _key, char* _buffer, size_t _length) {
    if (_key == nullptr) {
        return (0);
    }

    char key[64]{ 0 };
    snprintf(key, sizeof (key), "\"%s\"", _key);

    char* p = strstr(json_data, key);
    if (p == nullptr) { return (0); }
    p += strlen(key);

    p = strstr(p, ":");
    if (p == nullptr) { return (0); }
    p += 1;
    
    p = strstr(p, "\"");
    if (p == nullptr) { return (0); }
    p += 1;
    
    size_t i;
    for (i = 0; (i < (_length - 1)) && (*p != '\"'); i++) {
        _buffer[i] = *p++;
    }
    _buffer[i] = '\0';

    return (i);
}

void DevConfig::import_data(char* _json_string, KeyList* _key_list, size_t _size) {
    char user_par[64]{ 0 };
    size_t len;
    for (int i = 0; i < _size; i++) {
        len = DevConfig::json_get(_json_string, _key_list[i].key, user_par, sizeof (user_par));
        if (len > 0) {
            strncpy(_key_list[i].field, user_par, _key_list[i].length - 1);
        }
    }
}

char* DevConfig::allocate_command(char* _cmd) {
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

char* DevConfig::transact_command(const char* _ifac, const char* _cmd) {
    if (_cmd == nullptr) {
        printf("Error, tranaction not prepared!");
        return (nullptr);
    }

    int fd = -1;
    if (!DevConfig::open_interface(_ifac, fd)) {
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

    DevConfig::close_interface(fd);

    return (result);
}

bool DevConfig::read_data(const char* _ifac) {
    if (DevConfig::read_id(_ifac)) {
        if (DevConfig::read_config(_ifac)) {
            return (true);
        }
    }
    return (false);
}

bool DevConfig::read_config(const char* _ifac) {
    char* config_json = DevConfig::transact_command(_ifac, "/config");
    if (config_json != nullptr) {
        memset(&cfg, 0, sizeof (cfg));

        KeyList key_list[] = {
            { "version",              cfg.version,             sizeof (cfg.version)           },
            { "ssid",                 cfg.wifi_ssid,           sizeof (cfg.wifi_ssid)         },
            { "password",             cfg.wifi_password,       sizeof (cfg.wifi_password)     },
            { "wifi_channel",         cfg.wifi_channel,        sizeof (cfg.wifi_channel)      },
            { "mqtt_broker",          cfg.mqtt_broker,         sizeof (cfg.mqtt_broker)       },
            { "mqtt_username",        cfg.mqtt_username,       sizeof (cfg.mqtt_username)     },
            { "mqtt_password",        cfg.mqtt_password,       sizeof (cfg.mqtt_password)     },
            { "mqtt_enable",          cfg.mqtt_enable,         sizeof (cfg.mqtt_enable)       },
            { "display_layout",       cfg.display_layout,      sizeof (cfg.display_layout)    },
            { "display_param",        cfg.display_param,       sizeof (cfg.display_param)     },
            { "display_rotation",     cfg.display_rotoation,   sizeof (cfg.display_rotoation) },
            { "display_timeout",      cfg.display_timeout_s,   sizeof (cfg.display_timeout_s) },
            { "display_contrast",     cfg.display_contrast,    sizeof (cfg.display_contrast)  },
            { "sensor_type",          cfg.sensor_type,         sizeof (cfg.sensor_type)       },
            { "led_intensity",        cfg.led_intensity,       sizeof (cfg.led_intensity)     },
        };
        DevConfig::import_data(config_json, key_list, SIZEOFARRAY(key_list));

        free(config_json);
        return (true);
    }
    return (false);
}

bool DevConfig::read_id(const char* _ifac) {
    char* id_json = DevConfig::transact_command(_ifac, "/api/id");
    if (id_json != nullptr) {
        memset(&id, 0, sizeof (id));

        KeyList key_list[] = {
            { "identification",       id.identification,       sizeof (id.identification)       },
            { "manufacturer",         id.manufacturer,         sizeof (id.manufacturer)         },
            { "product",              id.product,              sizeof (id.product)              },
            { "device_serial_number", id.device_serial_number, sizeof (id.device_serial_number) },
            { "firmware_version",     id.firmware_version,     sizeof (id.firmware_version)     },
            { "firmware_date",        id.firmware_date,        sizeof (id.firmware_date)        },
            { "sensor_head",          id.head,                 sizeof (id.head)                 },
            { "head_serial_number",   id.head_serial,          sizeof (id.head_serial)          },
            { "chip_type",            id.chip_type,            sizeof (id.chip_type)            },
            { "wifi_station_mac",     id.wifi_station_mac,     sizeof (id.wifi_station_mac)     },
            { "wifi_ap_mac",          id.wifi_ap_mac,          sizeof (id.wifi_ap_mac)          },
            { "bluetooth_mac",        id.bluetooth_mac,        sizeof (id.bluetooth_mac)        },
            { "ip_addr",              id.ip_addr,              sizeof (id.ip_addr)              },
            { "rssi",                 id.rssi,                 sizeof (id.rssi)                 },
            { "tx_power",             id.tx_power,             sizeof (id.tx_power)             },
        };
        DevConfig::import_data(id_json, key_list, SIZEOFARRAY(key_list));

        free(id_json);
        return (true);
    }
    return (false);
}
