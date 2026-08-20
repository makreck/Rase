
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

#define FIRMWARE_CHIP_KEY_ESP32S3_DEVKIT_1              "ESP32-S3 Wroom"
#define FIRMWARE_CHIP_KEY_SEEED_STUDIO_XIAO_ESP32S3     "Seeed Studio XIAO ESP32-S3"
#define FIRMWARE_CHIP_KEY_WAVESHARE_ESP32S3_MINI        "Waveshare ESP32-S3 mini"
#define FIRMWARE_CHIP_KEY_DIYMORE_ESP32S3_SUPER_MINI    "diymore ESP32-S3"


typedef bool (*OTALoaderCB)(void* _user_param, int _id, float _progress, const char* _topic, const char* _message);


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

class MultiCmdParms {
    public:
        std::vector<DevConfig*>* device_list = nullptr;
        DevConfig*               device      = nullptr;
        OTALoaderCB              callback    = nullptr;
        void*                    user_param  = nullptr;
        char*                    command     = nullptr;

        MultiCmdParms(std::vector<DevConfig*>* _device_list, OTALoaderCB _callback, void* _user_param, const char* _command = nullptr) {
            set(_device_list, nullptr, _callback, _user_param, _command);
        }

        MultiCmdParms(DevConfig* _device, OTALoaderCB _callback, void* _user_param, const char* _command = nullptr) {
            set(nullptr, _device, _callback, _user_param, _command);
        }

        ~MultiCmdParms() {
            if (this->command != nullptr) {
                free(this->command);
                this->command = nullptr;
            }
        }

        void set(std::vector<DevConfig*>* _device_list, DevConfig* _device, OTALoaderCB _callback, void* _user_param, const char* _command) {
            this->device_list = _device_list;
            this->device      = _device;
            this->callback    = _callback;
            this->user_param  = _user_param;

            if (_command != nullptr) {
                size_t length = strlen(_command) + 1;
                this->command = (char*)malloc(length);
                if (this->command != nullptr) {
                    strncpy(this->command, _command, length);
                }
            } else {
                this->command = nullptr;
            }
        }
};

class EspTool {
    private:
        static void* _scanner_thread(void* _object);
        static void* _cmd_control_thread(void* _object);
        static void* _ota_control_thread(void* _object);
        static void* _cmd_exec_thread(void* _object);

    public:
        static bool find_tty_devices(std::vector<DevConfig*>* _device_list, pthread_mutex_t* _device_list_mutex);
        static int  open_serial_port(const char* _ifac, speed_t _baudrate = B115200);
        static bool force_reset_over_tty(int _fd);
        static bool load_binary(const char* _filename, uint8_t** _image, ssize_t* _length);
        static bool open_interface(const char* _ifac, int& _fd);
        static bool close_interface(int& _fd);
        static char* allocate_command(const char* _cmd);
        static char* transact_command(const char* _ifac, const char* _cmd);
        static bool read_config(const char* _ifac, DevConfig* _dev);
        static bool read_id(const char* _ifac, DevConfig* _dev);
        static bool read_data(const char* _ifac, DevConfig* _dev);
        static const char* find_matching_firmware_image(const char* _chip_type);
        static pthread_t update_all_devices(std::vector<DevConfig*>& _device_list, OTALoaderCB _callback, void* _user_param);
        static pthread_t transact_multi_device_command(std::vector<DevConfig*>& _device_list, const char* command, OTALoaderCB _callback, void* _user_param);
};

class OTAParms {
    public:
        int         id = 0;

        char        ifac[PATH_MAX]{ 0 };
        char        filename[PATH_MAX]{ 0 };

        OTALoaderCB callback     = nullptr;
        void*       user_param   = nullptr;

        uint8_t*    image_data   = nullptr;
        ssize_t     image_length = 0;

        OTAParms(int _id, const char* _ifac, const char* _filename, OTALoaderCB _callback = nullptr, void* _user_param = nullptr) {
            id = _id;
            
            if (_ifac != nullptr) {
                strncpy(ifac, _ifac, sizeof (ifac) - 1);
            }

            if (_filename != nullptr) {
                strncpy(filename, _filename, sizeof (filename) - 1);
                EspTool::load_binary(filename, &image_data, &image_length);
            }

            callback   = _callback;
            user_param = _user_param;
        }

        ~OTAParms() {
            if (image_data != nullptr) {
                free (image_data);
            }
        }

};
