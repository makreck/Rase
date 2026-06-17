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

void EvalCurve::init(size_t _length, int _index, ColorRef _color, float _line_width, RectEx& _rect) {
    index      = _index;
    color      = _color;
    line_width = _line_width;
    rc         = _rect;

    if (_length > 0) {
        length = std::max((size_t)LOG_EVAL_CURVE_LEN_MIN, std::min(_length, (size_t)LOG_EVAL_CURVE_LEN_MAX));
        size_t size = sizeof(EvalCurvePt) * length;
        data = (EvalCurvePt *)malloc(size);
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
        data[_index].pt.set(_x, _y);
        data[_index].flags = 0;
        return (true);
    }
    return (false);
}

bool EvalCurve::set_property(int _index, uint8_t _symbol, bool _startpoint, bool _endpoint) {
    if ((_index >= 0) && (_index < length)) {
        data[_index].flags        = 0;
        data[_index].symbol       = _symbol;
        data[_index].f_startpoint = _startpoint;
        data[_index].f_endpoint   = _endpoint;
        return (true);
    }
    return (false);
}

void EvalCurve::draw_stopper(cairo_t *_cr, double y) {
    const double stopper_dashes[2] = { 8.0, 4.0 };

    cairo_save(_cr);

    cairo_set_dash(_cr, stopper_dashes, SIZEOFARRAY(stopper_dashes), 0);
    cairo_set_source_rgba(_cr, 0.0, 0.0, 0.0, 0.25);
    cairo_set_antialias(_cr, CAIRO_ANTIALIAS_FAST);
    cairo_move_to(_cr, rc.x1(), y);
    cairo_line_to(_cr, rc.x2(), y);
    cairo_set_line_width(_cr, 0.5);
    cairo_stroke(_cr);

    cairo_restore(_cr);
}

void EvalCurve::draw(cairo_t *_cr, bool _foreground_curve) {
    if (data != nullptr) {
        if (length > 0) {
            float red   = CR_R(color);
            float green = CR_G(color);
            float blue  = CR_B(color);
            float alpha = CR_A(color);
            float width = (float)line_width;

            if (_foreground_curve == false) {
                alpha *= 0.75f;
            } else {
                width += 1.0f;
            }

            double x = data[0].pt.x;
            double y = data[0].pt.y;

            if (length > 1) {
                cairo_set_source_rgba(_cr, red, green, blue, alpha);
                cairo_set_line_width(_cr, width);
                cairo_move_to(_cr, x, y);

                int n = 0;
                for (size_t i = 0; i < length; i++) {
                    x = data[i].pt.x;
                    y = data[i].pt.y;

                    if (data[i].f_startpoint == 1) {
                        draw_stopper(_cr, y);
                        cairo_move_to(_cr, x, y);
                    } else {
                        cairo_line_to(_cr, x, y);
                        n++;

                        if (data[i].f_endpoint == 1) {
                            cairo_stroke(_cr);
                            n = 0;
                        
                            draw_stopper(_cr, y);
                        }
                    }
                }

                if (n > 0) {
                    cairo_stroke(_cr);
                }
            } else {
                cairo_set_source_rgba(_cr, red, green, blue, alpha);
                cairo_arc(_cr, x, y, 3, 0.0, 2.0 * M_PI);
                cairo_fill(_cr);
            }
        }
    }
}
