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
        ScaleFormat() {
            set(nullptr);
        }

        ScaleFormat(const ScaleFormat& _source) {
            set(&_source);
        }

        ScaleFormat(const ScaleFormat* _source) {
            set(_source);
        }

        void set(const ScaleFormat* _source) {
            if (_source != nullptr) {
                flags = _source->flags;
                color_ref = _source->color_ref;
            } else {
                flags = 0;
                dot_num = 1;
                line_width = 2;
                color_ref = C_RED;
            }
        }

        void reset(void) {
            set(nullptr);
        }

        void operator=(const ScaleFormat& source) {
            set(&source);
        }

        bool operator==(const ScaleFormat& source) {
            return ((color_ref == source.color_ref) && (flags == source.flags));
        }

        bool operator!=(const ScaleFormat& source) {
            return ((color_ref != source.color_ref) || (flags != source.flags));
        }

        void set_hidden(bool state) {
            f_hidden = state;
        }

        void set_scalemode(bool state) {
            f_scale_mode = state;
        }

        void set_integer(bool state) {
            f_integer = state;
        }

        void set_sign(bool state) {
            f_sign = state;
        }

        void set_binary(bool state) {
            f_binary = state;
        }

        void set_reverse(bool state) {
            f_reverse = state;
        }

        bool is_hidden(void) {
            return (f_hidden);
        }

        bool is_scalemode(void) {
            return (f_scale_mode);
        }

        bool is_integer(void) {
            return (f_integer);
        }

        bool is_sign(void) {
            return (f_sign);
        }

        bool is_binary(void) {
            return (f_binary);
        }

        bool is_reverse(void) {
            return (f_reverse);
        }

        void set_decimal_digits(int n) {
            dot_num = (uint32_t)(std::max(0, std::min(15, n)) & 0x0f);
        }

        int get_decimal_digits(void) {
            return ((int)dot_num & 0x0f);
        }

        void set_color_ref(ColorRef _color_ref) {
            color_ref = _color_ref;
        }

        ColorRef get_color_ref(void) {
            return (color_ref);
        }

        void set_line_width(float _line_width) {
            line_width = (uint32_t)(std::min(15.0f, std::max(0.0f, fabsf(_line_width) - 1.0f))) & 0x0f;
        }

        float get_line_width(void) {
            return ((float)line_width + 1.0f);
        }

        size_t get_number_len(float bottom, float top) {
            int nDecimals = 0;
            if (!is_integer()) {
                nDecimals = get_decimal_digits();
                if (nDecimals > 0)
                    nDecimals++;
            }

            int nCount = 0;
            if ((top > 0.0f) && (bottom >= 0.0f)) {
                int nRange = (int)((log10f(fabsf(top - bottom) + 0.5f) + 1.0f));
                nCount = nRange + nDecimals;
            } else {
                int nMin = (int)((log10f(fabsf(bottom) + 0.5f) + 1.0f));
                int nMax = (int)((log10f(fabsf(top) + 0.5f) + 1.0f));
                nCount = std::max(nMin + 1, nMax) + nDecimals;
            }

            return (nCount);
        }

};

class Scale : public MicroJsonObject {
    public:
        union {
            uint8_t buffer[256]{ 0 };
            struct {
                char  key[32];

                char  name[32];
                char  shortcut[4];
                char  unit[8];
                char  color[10];

                float bottom;
                float top;
                float value;
                float min;
                float max;

                float step;
                float zoom_begin;
                float zoom_end;
                
                float average;
                int   count;

                // Must follow struct ScaleFormat
                uint32_t flags;
                ColorRef color_ref;

                // Internal work-data
                float sum;
                void* user_data;
            };
        };

        const MicroJsonStruct meta_data[13] = {
            JSON_ITEM(Scale, name,      MicroJsonObjectType::obj_chars),
            JSON_ITEM(Scale, shortcut,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(Scale, unit,      MicroJsonObjectType::obj_chars),
            JSON_ITEM(Scale, color,     MicroJsonObjectType::obj_chars),
            JSON_ITEM(Scale, bottom,    MicroJsonObjectType::obj_float),
            JSON_ITEM(Scale, top,       MicroJsonObjectType::obj_float),
            JSON_ITEM(Scale, value,     MicroJsonObjectType::obj_float),
            JSON_ITEM(Scale, min,       MicroJsonObjectType::obj_float),
            JSON_ITEM(Scale, max,       MicroJsonObjectType::obj_float),
            JSON_ITEM(Scale, average,   MicroJsonObjectType::obj_float),
            JSON_ITEM(Scale, count,     MicroJsonObjectType::obj_int),
            JSON_ITEM(Scale, flags,     MicroJsonObjectType::obj_long),
            JSON_ITEM(Scale, color_ref, MicroJsonObjectType::obj_long),
        };
        JSON_GETTERS(Scale, meta_data);

    public:
        Scale(void) {
            init();
        }

        Scale(const Scale& _source) {
            set(&_source);
        }

        Scale(const Scale* _source) {
            set(_source);
        }

        Scale(const char* _key, const char* _name, const char* _shotcut, const char* _unit, float _bottom, float _top) {
            init();
            set_key(_key);
            set_name(_name);
            set_shortcut(_shotcut);
            set_unit(_unit);
            set_bottom(_bottom);
            set_top(_top);
        }

        ~Scale() {
            cleanup();
        }

        void init(void);
        void cleanup(void);
        void check(void);

        void operator=(const Scale& _source) {
            set(&_source);
        }

        void operator=(const Scale* _source) {
            set(_source);
        }

        void set(const Scale* _source);
        bool set_values(const Scale *_source);
        void set_defaults(void);
        void reset(void);
        void reset_zoom(void);

        void set_key(const char* _key = nullptr);
        const char* get_key(void);
        bool is_key_valid(void);

        void set_name(const char* _name = nullptr);
        const char* get_name(void);

        void set_shortcut(const char* _shortcut = nullptr);
        const char* get_shortcut(void);

        void set_unit(const char* _unit = nullptr);
        const char* get_unit(void);

        void set_bottom(float _buttom = 0.0f);
        float get_bottom(void);

        void set_top(float _top = 100.0f);
        float get_top(void);

        void set_value(float _value = 0.0f);
        float get_value(void);
        float get_normalized(void);
        float get_normalized(float _value);

        void set_step(float step);
        float get_step(void);

        float get_range(void);
        float get_min(void);
        float get_max(void);
                
        void set_zoom_window(float _zoom_begin, float _zoom_end);
        float get_zoom_begin(void);
        float get_zoom_end(void);
        float get_zoom_range(void);
        float get_zoom_normalized(void);
        float get_zoom_normalized(float _value);

        float get_average(void);
        float get_count(void);
        
        void distribute_color(bool ref2str = false);
        void set_color(const char* css_color);
        void set_color(ColorRef color);
        ColorRef get_color_ref(void);
        const char* get_color_css(void);
        float get_line_width(void);
        void set_line_width(float _width);
        ScaleFormat* get_format(void);

        void set_userdata(void* _user_data);
        void* get_userdata(void);
};
