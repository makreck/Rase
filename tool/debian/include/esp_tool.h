
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

#define OTA_CHUNK_SIZE              (4096)

#define DEFAULT_BAUDRATE            (B115200)
// #define ESP_MAX_BUFFER_SIZE         (4096)

// #define ENTER_BOOTLOADER_CMD "\x07\x07\x12\x20\x00\x00\x00\x00"
// #define EXIT_BOOTLOADER_CMD  "\x08\x07\x12\x20\x00\x00\x00\x00"

class DevConfig;


class EspTool {
    private:

    public:
        static bool find_interface(char* _ifac, size_t _length);
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

