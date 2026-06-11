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

class StaticParameter {
    private:
        void*   p_this = nullptr;
        int64_t value  = 0;
        void*   data   = nullptr;
        size_t  size   = 0;

        void store_data(void* _data, size_t _size) {
            size = _size;
            if (_data != nullptr) {
                if (_size == 0) {
                    data = _data;
                } else {
                    data = malloc(_size);
                    if (data != nullptr) {
                        memcpy(data, _data, _size);
                    }
                }
            }
        }

    public:
        StaticParameter(void* _this, void* _data, size_t _size = 0) {
            p_this = _this;
            value  = 0;
            store_data(_data, _size);
        }

        StaticParameter(void* _this, int64_t _value, void* _data = nullptr, size_t _size = 0) {
            p_this = _this;
            value  = _value;
            store_data(_data, _size);
        }

        ~StaticParameter() {
            if ((size > 0) && (data != nullptr)) {
                free(data);
            }
        }

        void*   get_this(void)  { return (p_this); }
        void*   get_data(void)  { return (data); }
        size_t  get_size(void)  { return (size); }
        int64_t get_value(void) { return (value); }
};
