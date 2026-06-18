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

class ScaleFormat {
    private:
        union {
            uint32_t flags;
            struct {
                uint32_t dot_num      : 4;
                uint32_t line_width   : 4;

                uint32_t f_sign       : 1;
                uint32_t f_reverse    : 1;
                uint32_t f_reserved_1 : 6;

                uint32_t f_hidden     : 1;
                uint32_t f_scale_mode : 1;
                uint32_t f_integer    : 1;
                uint32_t f_binary     : 1;
                uint32_t f_reserved_2 : 4;

                uint32_t f_reserved_3 : 8;
            };
        };

        ColorRef color_ref;

    public:
        void     reset(void);
        void     set(const ScaleFormat* _source);
        void     set_hidden(bool state);
        void     set_scalemode(bool state);
        void     set_integer(bool state);
        void     set_sign(bool state);
        void     set_binary(bool state);
        void     set_reverse(bool state);
        void     set_decimal_digits(int n);
        void     set_line_width(float _line_width);
        void     set_color_ref(ColorRef _color_ref);
        bool     is_hidden(void);
        bool     is_scalemode(void);
        bool     is_integer(void);
        bool     is_sign(void);
        bool     is_binary(void);
        bool     is_reverse(void);
        int      get_decimal_digits(void);
        float    get_line_width(void);
        size_t   get_number_len(float bottom, float top);
        ColorRef get_color_ref(void);
};
