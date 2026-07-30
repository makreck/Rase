
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

// enum class EspCmd {
//     sync      = 0x01,
//     mem_begin = 0x02,
//     mem_data  = 0x03,
//     mem_end   = 0x04,
//     run_flash = 0x05,
// };

enum class EspCmd {
    CMD_SYNC        = 0x07,
    CMD_READ_REG    = 0x08,
    CMD_WRITE_REG   = 0x0A,
    CMD_ERASE_FLASH = 0x41,
    CMD_WRITE_FLASH = 0x3B,
    CMD_READ_FLASH  = 0x32,
};

// Register addresses
#define REG_SYS_CONF                0x6000800C
#define REG_UART_CONF               0x60009014
#define REG_FLASH_CRYPT_CNTRL       0x6000A030

#define APP0_OFFSET                 0x10000
#define APP_SIZE                    0x180000

#define OTA_CHUNK_SIZE              (4096)

#define DEFAULT_BAUDRATE            (B115200)
#define ESP_MAX_BUFFER_SIZE         (4096)

// #define ENTER_BOOTLOADER_CMD "\x07\x07\x12\x20\x00\x00\x00\x00"
// #define EXIT_BOOTLOADER_CMD  "\x08\x07\x12\x20\x00\x00\x00\x00"

class PartitionEntry {
    public:
        const char* name;
        uint8_t     type;
        uint8_t     subtype;
        uint32_t    offset;
        uint32_t    size;
        uint32_t    flags;
};

typedef bool (*EspLoaderCB)(void* _user_param, const char* _topic, const char* _message);

class EspTool {
    private:
        static const PartitionEntry partitions[6];
        static const char* ota_put_req_string;

        static bool ota_loader(const char* _ip_addr, uint8_t* _firmware_image, ssize_t _size, EspLoaderCB _callback, void* _user_param);
        static void* _loader_thread(void* _object);

    public:
        static bool find_interface(char* _ifac, size_t _length);
        static int  open_serial_port(const char* _ifac, speed_t _baudrate = B115200);
        static bool force_reset_over_tty(int _fd);
        static bool load_binary(const char* _filename, uint8_t** _image, ssize_t* _length);
        static pthread_t firmware_loader(const char* _ifac, const char* _filename, EspLoaderCB _callback = nullptr, void* _user_param = nullptr);

};

class EspToolParms {
    public:
        char        ifac[PATH_MAX]{ 0 };
        char        filename[PATH_MAX]{ 0 };

        EspLoaderCB callback     = nullptr;
        void*       user_param   = nullptr;

        uint8_t*    image_data   = nullptr;
        ssize_t     image_length = 0;

        EspToolParms(const char* _ifac, const char* _filename, EspLoaderCB _callback = nullptr, void* _user_param = nullptr) {
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

        ~EspToolParms() {
            if (image_data != nullptr) {
                free (image_data);
            }
        }

};

