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

void EvalCurve::init(size_t _length, int _index, ColorRef _color, float _line_width) {
    index = _index;
    color = _color;
    line_width = _line_width;
    if (_length > 0) {
        length = std::max((size_t)LOG_EVAL_CURVE_LEN_MIN, std::min(_length, (size_t)LOG_EVAL_CURVE_LEN_MAX));
        size_t size = sizeof(PointF) * length;
        data = (PointF *)malloc(size);
        memset(data, 0, size);
    }
}

void EvalCurve::cleanup(void) {
    length = 0;
    if (data != nullptr) {
        free(data);
        data = nullptr;
    }
}

bool EvalCurve::set(int _index, double _x, double _y) {
    if ((_index >= 0) && (_index < length)) {
        data[_index].set(_x, _y);
        return (true);
    }
    return (false);
}

void EvalCurve::draw(cairo_t *_cr, ColorRef _color, int line_width) {
    if (data != nullptr) {
        if (length > 1) {
            cairo_set_source_rgba(_cr, CR_R(_color), CR_G(_color), CR_B(_color), CR_A(_color));
            cairo_set_line_width(_cr, (double)line_width);
            cairo_move_to(_cr, data[0].x, data[0].y);
            for (size_t i = 1; i < length; i++) {
                cairo_line_to(_cr, data[i].x, data[i].y);
            }
            cairo_stroke(_cr);
        } else {
            // Only one measurment point in curve. @todo: draw a dot or symbol.
        }
    }
}
