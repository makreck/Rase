
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

#pragma once

#define MAX_TTY          (100)
#define DEFAULT_BAUDRATE (B115200)

class DevConfig;


class ScanTTY {
    public:
        DevConfig*               device            = nullptr;
        std::vector<DevConfig*>* device_list       = nullptr;
        pthread_mutex_t*         device_list_mutex = nullptr;

        ScanTTY(DevConfig* _device, std::vector<DevConfig*>* _device_list, pthread_mutex_t* _device_list_mutex) {
            this->device            = _device;
            this->device_list       = _device_list;
            this->device_list_mutex = _device_list_mutex;
        }
};

class EspTool {
    private:
        static void* _scanner_thread(void* _object);

    public:
        static bool find_interface(char* _ifac, size_t _length);
        static bool find_tty_devices(std::vector<DevConfig*>* _device_list, pthread_mutex_t* _device_list_mutex);
        static int  open_serial_port(const char* _ifac, speed_t _baudrate = B115200);
        static bool force_reset_over_tty(int _fd);
        static bool load_binary(const char* _filename, uint8_t** _image, ssize_t* _length);
        static bool open_interface(const char* _ifac, int& _fd);
        static bool close_interface(int& _fd);
        static char* allocate_command(char* _cmd);
        static char* transact_command(const char* _ifac, const char* _cmd);
        static bool read_config(const char* _ifac, DevConfig* _dev);
        static bool read_id(const char* _ifac, DevConfig* _dev);
        static bool read_data(const char* _ifac, DevConfig* _dev);
};

