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

class LogHeader {
    private:
        union {
            uint8_t buffer[1024]{ 0 };
            struct {
                char      title_string[64];
                uint32_t  magic_id;
                uint32_t  file_version;
                uint64_t   auto_update_threshold_ms;
                uint64_t   header_update_count;
                ProductID product_info;
                
                union {
                    uint32_t flags;
                    struct {
                        uint32_t modified  : 1;
                        uint32_t reserved0 : 7;
                        uint32_t time_zone : 8;
                        uint32_t reserved2 : 8;
                        uint32_t reserved3 : 8;
                    };
                };
                
                struct {
                    double created = 0.0;
                    double updated = 0.0;
                } timestamp;

            } data;
        };

    public:
        LogHeader(void) {
            clear();
        }

        LogHeader(const ProductID* _product_info) {
            clear();
            set(_product_info);
        }

        ~LogHeader() {
        }

        void clear(void);
        bool set(const ProductID* _product_info);
        bool set_header_update_threshold_ms(float _time_s);
        uint64_t get_auto_update_threshold_ms(void);
        void set_modified(bool set_bit = true);
        void clear_modified(void);
        bool is_modified(void);
        bool register_update(void);
        bool must_update(uint64_t& update_timestamp);
        double get_creation_timestamp(void);
        double get_update_timestamp(void);
        double get_utc_offset_timecode(void);

};
