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

#include "includes.h"

void Scale::init(void) {
    set(nullptr);
}

void Scale::cleanup(void) {
}

bool Scale::set_values(const Scale *_source) {
    bool modified = false;
    if (_source != nullptr) {
        modified  = ((value != _source->value) || (count != _source->count));
        value     = _source->value;
        min       = _source->min;
        max       = _source->max;
        average   = _source->average;
        count     = _source->count;
        sum       = _source->sum;
        color_ref = _source->color_ref;
        distribute_color(true);
    }
    return (modified);
}

void Scale::set(const Scale *_source) {
    if (_source != nullptr) {
        set_key(_source->key);
        set_name(_source->name);
        set_shortcut(_source->shortcut);
        set_unit(_source->unit);
        flags     = _source->flags;
        color_ref = _source->color_ref;
        bottom    = _source->bottom;
        top       = _source->top;
        value     = _source->value;
        min       = _source->min;
        max       = _source->max;
        average   = _source->average;
        count     = _source->count;
        sum       = _source->sum;
    } else {
        set_key();
        set_name();
        set_shortcut();
        set_unit();
        
        flags     = 0;
        color_ref = 0;
        bottom    = 0.0;
        top       = 0.0;
        value     = 0.0;
        min       = 0.0;
        max       = 0.0;
        average   = 0.0;
        count     = 0;
        sum       = 0.0;
    }
    distribute_color(true);
}

void Scale::set_defaults(void) {
    set_key("default_scale");
    set_name("Relative");
    set_shortcut("REL");
    set_unit("%");
    set_color("#ff0000ff");
    flags      =      0;
    color_ref  =      0;
    bottom     =   0.0f;
    top        = 100.0f;
    value      =   0.0f;
    min        = 100.0f;
    max        =   0.0f;
    step       =   1.0f;
    zoom_begin =   0.0f;
    zoom_end   = 100.0f;
    average    =   0.0f;
    count      =      0;
    sum        =   0.0f;
    distribute_color();
}

void Scale::reset(void) {
    sum        =   0.0f;
    count      =      0;
    value      = bottom;
    zoom_begin = bottom;
    zoom_end   = top;
    distribute_color();
}

void Scale::reset_zoom(void) {
    zoom_begin = bottom;
    zoom_end   = top;
}

void Scale::set_key(const char *_key) {
    memset(key, 0, sizeof (key));
    if (_key != nullptr) {
        strncpy(key, _key, sizeof (key) - 1);
    }
}

bool Scale::is_key_valid(void) {
    return (key[0] != '\0');
}

const char* Scale::get_key(void) {
    return (key);
}

void Scale::set_name(const char *_name) {
    memset(name, 0, sizeof (name));
    if (_name != nullptr) {
        strncpy(name, _name, sizeof (name) - 1);
    }
}

const char* Scale::get_name(void) {
    return (name);
}

void Scale::set_shortcut(const char *_shortcut) {
    memset(shortcut, 0, sizeof (shortcut));
    if (_shortcut != nullptr) {
        strncpy(shortcut, _shortcut, sizeof (shortcut) - 1);
    }
}

const char* Scale::get_shortcut(void) {
    return (shortcut);
}

void Scale::set_unit(const char *_unit) {
    memset(unit, 0, sizeof (unit));
    if (_unit != nullptr) {
        strncpy(unit, _unit, sizeof (unit) - 1);
    }
}

const char* Scale::get_unit(void) {
    return (unit);
}

void Scale::check(void) {
    if (bottom == top) {
        float d = (step == 0.0f) ? 0.5f : (step * 0.5f);
        bottom = bottom - d;
        top    = bottom + d;
    } else if (bottom > top) {
            float x = bottom;
            bottom = top;
            top = x;
    }

    step = fabsf(step);
    if (step == 0.0f) {
        step = (top - bottom) / 100.0f;
    } else {
        float _range = get_range();
        step = std::min(_range / 3.0f, std::max(_range * 0.0001f, step));
    }

    if (zoom_begin == zoom_end) {
        zoom_begin = bottom;
        zoom_end = top;
    } else if (zoom_begin > zoom_end) {
        float x = zoom_begin;
        zoom_begin = zoom_end;
        zoom_end = x;
    }

    zoom_begin = std::max(bottom, zoom_begin);
    zoom_end   = std::min(top, zoom_end);

    if (fabsf(zoom_end - zoom_begin) < fabsf(step)) {
        float d = fabsf(step) * 0.5f;
        zoom_begin -= d;
        zoom_end   += d;
    }
}

void Scale::set_bottom(float _bottom) {
    bottom = _bottom;
    check();
}

float Scale::get_bottom(void) {
    return (bottom);
}

void Scale::set_top(float _top) {
    top = _top;
    check();
}

float Scale::get_top(void) {
    return (top);
}

float Scale::get_range(void) {
    if (top == bottom) {
        check();
    }
    return (top - bottom);
}

void Scale::set_value(float _value) {
    value = std::max(bottom, std::min(top, value));
    sum += value;
    count += 1.0f;
}

float Scale::get_value(void) {
    return (value);
}

void Scale::set_step(float _step) {
    step = _step;
    check();
}

float Scale::get_step(void) {
    return (step);
}

float Scale::get_min(void) {
    return (min);
}

float Scale::get_max(void) {
    return (max);
}

float Scale::get_normalized(void) {
    return (get_normalized(get_value()));
}

float Scale::get_normalized(float _value) {
    return ((_value - get_bottom()) / get_range());
}

float Scale::get_zoom_normalized(void) {
    return (get_zoom_normalized(get_value()));
}

float Scale::get_zoom_normalized(float _value) {
    return ((_value - get_zoom_begin()) / get_zoom_range());
}

void Scale::set_zoom_window(float _zoom_begin, float _zoom_end) {
    zoom_begin = _zoom_begin;
    zoom_end   = _zoom_end;
    check();
}

float Scale::get_zoom_begin(void) {
    return (zoom_begin);
}

float Scale::get_zoom_end(void) {
    return (zoom_end);
}

float Scale::get_zoom_range(void) {
    if (zoom_begin == zoom_end) {
        check();
    }
    return (zoom_end - zoom_begin);
}

float Scale::get_average(void) {
    if (count > 0) {
        return (average);
    }
    return (bottom);
}

float Scale::get_count(void) {
    return (count);
}

ScaleFormat* Scale::get_format(void) {
    return ((ScaleFormat*)&flags);
}

void Scale::distribute_color(bool ref2str) {
    if (ref2str == true) {
        snprintf(color, sizeof (color), "#%2.2x%2.2x%2.2x%2.2x",
            (unsigned int)RGBA_R(color_ref), (unsigned int)RGBA_G(color_ref), (unsigned int)RGBA_B(color_ref), (unsigned int)RGBA_A(color_ref));
    } else {
        char hex[6]{ 0 };
        hex[0] = '0';
        hex[1] = 'x';

        int rgba[] = { RGBA_R(color_ref), RGBA_G(color_ref), RGBA_B(color_ref), RGBA_A(color_ref) };
        int n = 0;
        for (int i = 1; (i < strlen(color)) && (n < 4); i += 2) {
            hex[2] = color[i + 0];
            hex[3] = color[i + 1];
            rgba[n++] = strtoul(hex, nullptr, 0);
        }
        
        if (n == 3) {
            rgba[3] = 0xff;
        }

        color_ref = (ColorRef)RGBA(rgba[0], rgba[1], rgba[2], rgba[3]);
    }
}

void Scale::set_color(const char* _css_color) {
    if (_css_color != nullptr) {
        size_t len = strlen(_css_color);
        if ((_css_color[0] == '#') && ((len == 7) || (len == 9))) {
            memcpy(color, _css_color, len);
            if (len == 7) {
                color[len++] = 'f';
                color[len++] = 'f';
            }
            color[len] = 0;
        }
    }
    distribute_color(false);
}

void Scale::set_color(ColorRef _color_ref) {
    color_ref = _color_ref;
    distribute_color(true);
}

ColorRef Scale::get_color_ref(void) {
    return (color_ref);
}

const char* Scale::get_color_css(void) {
    return (color);
}

void Scale::set_userdata(void *_user_data) {
    user_data = _user_data;
}

void* Scale::get_userdata(void) {
    return (user_data);
}

float Scale::get_line_width(void) {
    return (get_format()->get_line_width());
}

void Scale::set_line_width(float _width) {
    get_format()->set_line_width(_width);
}
