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

void EvalCurve::init(size_t _length, int _slot, ProductID* _product_id, Scale* _scale) {
    length = (uint32_t)(std::max((size_t)LOG_EVAL_CURVE_LEN_MIN, std::min(_length, (size_t)LOG_EVAL_CURVE_LEN_MAX)) & 0xffff);
    slot   = (uint32_t)(_slot & 0x0f);
    
    product_id.set(_product_id);
    scale.set(_scale);

    size_t size = sizeof(EvalCurvePt) * std::max((size_t)1, get_length());
    data = (EvalCurvePt *)malloc(size);
    memset(data, 0, size);
}

void EvalCurve::cleanup(void) {
    length = 0;
    if (data != nullptr) {
        free(data);
        data = nullptr;
    }
}

bool EvalCurve::set(int _index, double _timecode, float _value, float _x, float _y) {
    if ((_index >= 0) && (_index < get_length())) {
        data[_index].m.pt.set(_x, _y);
        data[_index].m.timecode = _timecode;
        data[_index].m.value    = _value;
        data[_index].m.f_used   = 1;
        return (true);
    }
    return (false);
}

PointF* EvalCurve::get_point(int _index) {
    if ((_index >= 0) && (_index < get_length())) {
        return (&data[_index].m.pt);
    }
    return (nullptr);
}

double EvalCurve::get_timecode(int _index) {
    if ((_index >= 0) && (_index < get_length())) {
        return (data[_index].m.timecode);
    }
    return (0.0);
}

float EvalCurve::get_value(int _index) {
    if ((_index >= 0) && (_index < get_length())) {
        return (data[_index].m.value);
    }
    return (0.0f);
}

float EvalCurve::get_first_value(void) {
    if (get_length() > 0) {
        return (data[0].m.value);
    }
    return (0.0f);
}

float EvalCurve::get_last_value(void) {
    if (get_length() > 0) {
        int i = get_length() - 1;
        return (data[i].m.value);
    }
    return (0.0f);
}

void EvalCurve::set_selected(bool _state) {
    f_selected = (_state) ? 1 : 0;
}

bool EvalCurve::is_selected(void) {
    return (f_selected == 1);
}

bool EvalCurve::is_used(int _index) {
    if ((_index >= 0) && (_index < get_length())) {
        return (data[_index].m.f_used == 1);
    }
    return (false);
}

bool EvalCurve::is_begin(int _index) {
    if ((_index >= 0) && (_index < get_length())) {
        return (data[_index].m.f_startpoint == 1);
    }
    return (false);
}

bool EvalCurve::is_end(int _index) {
    if ((_index >= 0) && (_index < get_length())) {
        return (data[_index].m.f_endpoint == 1);
    }
    return (false);
}

bool EvalCurve::is_single(int _index) {
    if ((_index >= 0) && (_index < get_length())) {
        return ((data[_index].m.f_startpoint == 1) && (data[_index].m.f_endpoint == 1));
    }
    return (false);
}

bool EvalCurve::set_begin(int _index, bool state) {
    if ((_index >= 0) && (_index < get_length())) {
        data[_index].m.f_startpoint = (state) ? 1 : 0;
        return (true);
    }
    return (false);
}

bool EvalCurve::set_end(int _index, bool state) {
    if ((_index >= 0) && (_index < get_length())) {
        data[_index].m.f_endpoint = (state) ? 1 : 0;
        return (true);
    }
    return (false);
}

bool EvalCurve::set_symbol(int _index, uint8_t _symbol) {
    if ((_index >= 0) && (_index < get_length())) {
        data[_index].m.symbol = _symbol;
        return (true);
    }
    return (false);
}

int EvalCurve::get_slot(void) {
    return ((int)(slot & 0x0f));
}

size_t EvalCurve::get_length(void) {
    return ((size_t)(length & 0xffff));
}

Scale* EvalCurve::get_scale(void) {
    return (&scale);
}

ProductID* EvalCurve::get_product_id(void) {
    return (&product_id);
}

void EvalCurve::draw_stopper(cairo_t *_cr, RectEx& rc, double y) {
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

void EvalCurve::draw(cairo_t *_cr, RectEx& _rc, bool _foreground_curve) {
    if (data != nullptr) {
        if (length > 0) {
            ColorRef color = scale.get_color_ref();
            float red   = CR_R(color);
            float green = CR_G(color);
            float blue  = CR_B(color);
            float alpha = CR_A(color);

            float width = scale.get_line_width();

            if (_foreground_curve == false) {
                alpha *= 0.75f;
            } else {
                width += 1.0f;
            }

            double x = data[0].m.pt.x * (float)_rc.width  + _rc.x;
            double y = data[0].m.pt.y * (float)_rc.height + _rc.y;

            cairo_move_to(_cr, x, y);

            cairo_set_source_rgba(_cr, red, green, blue, alpha);
            cairo_set_line_width(_cr, width);

            int n = 0;
            for (size_t i = 0; i < get_length(); i++) {
                if (!is_used(i)) continue;

                x = data[i].m.pt.x * (float)_rc.width  + _rc.x;
                y = data[i].m.pt.y * (float)_rc.height + _rc.y;

                if (is_single(i)) {
                    if (n > 0) {
                        cairo_stroke(_cr);
                        n = 0;
                    }
                    cairo_move_to(_cr, x, y);
                    cairo_arc(_cr, x, y, 3, 0.0, 2.0 * M_PI);
                    cairo_fill(_cr);
                    cairo_stroke(_cr);
                } else if (is_begin(i)) {
                    draw_stopper(_cr, _rc, y);
                    cairo_move_to(_cr, x, y);
                } else {
                    cairo_line_to(_cr, x, y);
                    n++;

                    if (is_end(i)) {
                        cairo_stroke(_cr);
                        n = 0;

                        draw_stopper(_cr, _rc, y);
                    } else if (n >= LOG_EVAL_MAX_STROKE) {
                        cairo_stroke(_cr);
                        cairo_move_to(_cr, x, y);
                        n = 0;
                    }
                }
            }

            if (n > 0) {
                cairo_stroke(_cr);
            }
        }
    }
}

bool EvalCurve::clean_curve(void) {
    int k = 0;
    int i = 0;
    for (i = 0; i < get_length(); i++) {
        if (data[i].m.f_used == 1) {
            if (k != i) {
                data[k++].set(&data[i]);
            }
        }
    }
    length = k;
    return (k != i);
}

float EvalCurve::get_value_at_timecode(double _timecode, double* _exact_time) {
    int len = get_length();
    if (len > 0) {
        int i = 0;
        if (_timecode < data[0].m.timecode) {
            i = 0;
        } else if (_timecode > data[len - 1].m.timecode) {
            i = len - 1;
        } else {
            i = len / 2;
            int n = (i / 2) + 1;
            do {
                if (_timecode < data[i].m.timecode) {
                    i -= n;
                } else if (_timecode > data[i].m.timecode) {
                    i += n;
                } else {
                    break;
                }
                n /= 2;
            } while (n > 0);
        }
        double d = fabs(_timecode - data[i].m.timecode);
        if ((d >= TC_MILLISEC) && (i > 0) && (i < (len - 1))) {
            if (fabs(_timecode - data[i - 1].m.timecode) < d) {
                i--;
            } else if (fabs(_timecode - data[i + 1].m.timecode) < d) {
                i++;
            }
        }
        if (_exact_time != nullptr) {
            *_exact_time = data[i].m.timecode;
        }
        return (data[i].m.value);
    }
    return (0.0f);
}
