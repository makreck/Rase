
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


typedef bool (*EspLoaderCB)(void* _user_param);

class EspTool {
    private:
        static const PartitionEntry partitions[6];

    public:
        static bool find_interface(char* _ifac, size_t _length);
        static int  open_serial_port(const char* _ifac, speed_t _baudrate = B115200);
        static bool force_reset_over_tty(int _fd);
        static bool load_binary(const char* _filename, uint8_t** _image, ssize_t* _length);

        static bool upload_2nd_level(const char* _filename, const char* _ifac, EspLoaderCB _callback = nullptr, void* _user_param = nullptr);
        static bool send_command(int _fd, EspCmd _cmd, uint8_t* _data, size_t _length);
        static bool enter_2nd_level_bootloader(int _fd);
        static bool firmware_loader(const char* _ifac, const char* _filename);
        static bool ota_loader(const char* _ip_addr, uint8_t* _firmware_image, ssize_t _size);

};

