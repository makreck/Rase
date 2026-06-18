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

void ScaleFormat::set(const ScaleFormat *_source) {
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

void ScaleFormat::reset(void) {
    set(nullptr);
}

void ScaleFormat::set_hidden(bool state) {
    f_hidden = state;
}

void ScaleFormat::set_scalemode(bool state) {
    f_scale_mode = state;
}

void ScaleFormat::set_integer(bool state) {
    f_integer = state;
}

void ScaleFormat::set_sign(bool state) {
    f_sign = state;
}

void ScaleFormat::set_binary(bool state) {
    f_binary = state;
}

void ScaleFormat::set_reverse(bool state) {
    f_reverse = state;
}

bool ScaleFormat::is_hidden(void) {
    return (f_hidden);
}

bool ScaleFormat::is_scalemode(void) {
    return (f_scale_mode);
}

bool ScaleFormat::is_integer(void) {
    return (f_integer);
}

bool ScaleFormat::is_sign(void) {
    return (f_sign);
}

bool ScaleFormat::is_binary(void) {
    return (f_binary);
}

bool ScaleFormat::is_reverse(void) {
    return (f_reverse);
}

void ScaleFormat::set_decimal_digits(int n) {
    dot_num = (uint32_t)(std::max(0, std::min(15, n)) & 0x0f);
}

int ScaleFormat::get_decimal_digits(void) {
    return ((int)dot_num & 0x0f);
}

void ScaleFormat::set_color_ref(ColorRef _color_ref) {
    color_ref = _color_ref;
}

ColorRef ScaleFormat::get_color_ref(void) {
    return (color_ref);
}

void ScaleFormat::set_line_width(float _line_width) {
    line_width = (uint32_t)(std::min(15.0f, std::max(0.0f, fabsf(_line_width) - 1.0f))) & 0x0f;
}

float ScaleFormat::get_line_width(void) {
    return ((float)line_width + 1.0f);
}

size_t ScaleFormat::get_number_len(float bottom, float top) {
    int nDecimals = 0;
    if (!is_integer()) {
        nDecimals = get_decimal_digits();
        if (nDecimals > 0) {
            nDecimals++;
        }
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
