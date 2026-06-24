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

class Scale {
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
                float sum;
                int   count;

                union {
                    struct {
                        uint32_t flags;
                        ColorRef color_ref;
                    };
                    ScaleFormat format;
                };

                union { 
                    void* ptr;
                    double f;
                } user_data;

            };
        };

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

        Scale(ScaleJson* _source) {
            import(_source);
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

        void import(ScaleJson* _source);

        void clear(void);
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

        void set_userdata(void* _ptr);
        void* get_userdata(void);
        void set_userdata(double _f);
        void get_userdata(double* _pf);
};
