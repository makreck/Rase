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

#define LOG_FRAME_MAGIC_BYTE (0x77)

class LogFrame {
    private:
        union {
            uint8_t buffer[16]{ 0 };
            struct {
                double timecode;
                float  value;
                union {
                    uint32_t flags;
                    struct {
                        uint32_t slot      : 4;
                        uint32_t symbol    : 4;
                        uint32_t reserved1 : 8;
                        uint32_t magic_id  : 8;
                        uint32_t reserved3 : 7;
                        uint32_t valid     : 1;
                    };
                };
            };
        };

    public:
        LogFrame(void) {
            clear();
        }

        LogFrame(int _slot, double _timecode, float _value) {
            clear();
            valid = (set_slot(_slot) && set_timecode(_timecode) && set_value(_value));
        }

        LogFrame(int _slot, float _value) {
            clear();
            valid = (set_slot(_slot) && set_timecode(0.0) && set_value(_value));
        }

        ~LogFrame() {
            value = false;
        }

        void operator=(const LogFrame& _source) {
            set(_source);
        }

        void operator=(const LogFrame* _source) {
            set(_source);
        }

        void operator=(const double& _timecode) {
            set_timecode(_timecode);
        }

        void operator=(const float& _value) {
            set_value(_value);
        }

        void operator=(const int& _slot) {
            set_slot(_slot);
        }

        void   clear(void);

        bool   set(const LogFrame& _source);
        bool   set(const LogFrame* _source);
        bool   set_timecode(double _timecode);
        bool   set_value(float _value);
        bool   set_slot(int _slot);
        bool   set_symbol(int _symbol);

        double get_timecode(void);
        float  get_value(void);
        int    get_slot(void);
        int    get_symbol(void);
        bool   get(LogFrame& _target);
        bool   get(LogFrame* _target);


};
