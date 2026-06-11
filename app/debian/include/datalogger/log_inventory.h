/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
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

class LogInventory {
    private:
        uint8_t userdata[LOG_USERDATA_MAX]{ 0 };
        uint32_t filler = 0;
        uint32_t userdata_length = 0;
        uint8_t reserved[376]{ 0 };
        Scale slot[LOG_SLOT_MAX];

    public:
        LogInventory() {
            init();
        }

        ~LogInventory() {
            cleanup();
        }

        void init(void);
        void cleanup(void);
        
        void clear(void);

        int add_slot(Scale* _channel_data);
        const Scale* get_slot(int _index);
        const Scale* get_slot(const char* _key);
        int find_slot(const char* key);
        int get_free_slot(void);

        size_t set_userdata(const void* _userdata, size_t _length);
        size_t get_userdata(void* _userdata, size_t _length);
        const void* get_userdata(size_t* _length = nullptr);

};
