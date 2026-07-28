
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

enum class EspCmd {
    sync      = 0x01,
    mem_begin = 0x02,
    mem_data  = 0x03,
    mem_end   = 0x04,
    run_flash = 0x05,
    reserved  = 0x06,
    ack       = 0x07,
    nak       = 0x08,
    get_info  = 0x0b,
};

#define DEFAULT_BAUDRATE    (B115200)
#define ESP_MAX_BUFFER_SIZE (4096)
#define ESP_RESPONSE_SIZE   (10)

#define ENTER_BOOTLOADER_CMD "\x07\x07\x12\x20\x00\x00\x00\x00"
#define EXIT_BOOTLOADER_CMD  "\x08\x07\x12\x20\x00\x00\x00\x00"

typedef bool (*EspLoaderCB)(void* _user_param);

class EspTool {
    private:
        struct {
            char        ifac[PATH_MAX]{ 0 };
            int         fd = -1;
            EspLoaderCB callback;
            void*       user_param;
        } m;

        void init(const char* _ifac, EspLoaderCB _callback, void* _user_param);
        void cleanup(void);

    public:
        EspTool(const char* _ifac, EspLoaderCB _callback, void* _user_param) {
            init(_ifac, _callback, _user_param);
        }

        ~EspTool() {
            cleanup();
        }

        static bool find_interface(char* _ifac, size_t _length);
        static int  open_serial_port(const char* _ifac, speed_t _baudrate = B115200);
        static bool force_reset_over_tty(int _fd);
        static bool load_binary(const char* _filename, uint8_t** _image, ssize_t* _length);
        static bool upload_2nd_level(const char* _filename, const char* _ifac, EspLoaderCB _callback = nullptr, void* _user_param = nullptr);

};
