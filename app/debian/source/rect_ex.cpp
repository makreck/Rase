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

void RectEx::operator=(const GdkRectangle &_source) {
    set(_source.x, _source.y, _source.width, _source.height);
}

void RectEx::operator=(const RectEx* _source) {
    if (_source != nullptr) {
        set(_source->x, _source->y, _source->width, _source->height);
    } else {
        set(0, 0, 0, 0);
    }
}

void RectEx::operator=(const RectEx& _source) {
    set(_source.x, _source.y, _source.width, _source.height);
}

bool RectEx::operator==(const RectEx& _source) {
    return ((x == _source.x) && (y == _source.y) && (width == _source.width) && (height == _source.height));
}

bool RectEx::operator!=(const RectEx& _source) {
    return ((x != _source.x) || (y != _source.y) || (width != _source.width) || (height != _source.height));
}

int RectEx::x1(void) {
    return (x);
}

int RectEx::y1(void) {
    return (y);
}

int RectEx::x2(void) {
    return (x + width);
}

int RectEx::y2(void) {
    return (y + height);
}

void RectEx::set(RectEx& _source) {
    set(_source.x, _source.y, _source.width, _source.height);
}

void RectEx::set(RectEx* _source) {
    if (_source != nullptr) {
        set(_source->x, _source->y, _source->width, _source->height);
    } else {
        set(0, 0, 0, 0);
    }
}

void RectEx::set(double _x, double _y, double _width, double _height) {
    x = (int)(_x);
    y = (int)(_y);
    width = (int)(_width);
    height = (int)(_height);
}

void RectEx::set(GtkWidget* _widget) {
    if (_widget == nullptr) {
        set(0, 0, 1, 1);
    } else {
        set(0, 0, gtk_widget_get_allocated_width(_widget), gtk_widget_get_allocated_height(_widget));
    }
}

void RectEx::set(PointF* _pt, double _width, double _height) {
    if (_pt != nullptr) {
        x      = (int)_pt->x;
        y      = (int)_pt->y;
        width  = (int)(_width);
        height = (int)(_height);
    }
}

void RectEx::normalize(void) {
    width -= x;
    x = 0;
    height -= y;
    y = 0;
}

int RectEx::center_x(void) {
    return ((width / 2) + x);
}

int RectEx::center_y(void) {
    return ((height / 2) + y);
}

void RectEx::clr(void) {
    set(0, 0, 0, 0);
}

void RectEx::set_left(const char* _string) {
    x = (int)format_string(_string);
}

void RectEx::set_top(const char* _string) {
    y = (int)format_string(_string);
}

void RectEx::set_right(const char* _string) {
    width = (int)format_string(_string) - x;
}

void RectEx::set_bottom(const char* _string) {
    height = (int)format_string(_string) - y;
}

double RectEx::format_string(const char* _string) {
    double value = 0.0;
    if (_string != nullptr) {
        value = atof(_string);
        if (value == 0.0) {
            value = (double)strtoul(_string, nullptr, 0);
        }
    }
    return (value);
}

void RectEx::shrink_width(double _n) {
    expand_width(-(_n));
}

void RectEx::shrink_height(double _n) {
    expand_height(-(_n));
}

void RectEx::shrink(double _n) {
    shrink_width(_n);
    shrink_height(_n);
}

void RectEx::expand_width(double _n) {
    x -= (int)(_n);
    width += (int)(_n + _n);
}

void RectEx::expand_height(double _n) {
    y -= (int)(_n);
    height += (int)(_n + _n);
}

void RectEx::expand(double _n) {
    expand_width(_n);
    expand_height(_n);
}

void RectEx::offset(double _x, double _y) {
    x += _x;
    y += _y;
}

void RectEx::set(cairo_t* _cr) {
    if (_cr != nullptr) {
        cairo_rectangle(_cr, x, y, width, height);
    }
}

void RectEx::clip(cairo_t* _cr) {
    if (_cr != nullptr) {
        cairo_rectangle(_cr, x, y, width, height);
        cairo_clip(_cr);
    }
}

cairo_pattern_t* RectEx::linear_gradient(RectDir _direction, ColorRef _cBegin, ColorRef _cEnd) {
    cairo_pattern_t* pattern;
    if (_direction == RectDir::vertical) {
        pattern = cairo_pattern_create_linear(x1(), y1(), x1(), y2());
    } else {
        pattern = cairo_pattern_create_linear(x1(), y1(), x2(), y1());
    }
    cairo_pattern_add_color_stop_rgba(pattern, 0.0, CR_RED(_cBegin), CR_GREEN(_cBegin), CR_BLUE(_cBegin), CR_ALPHA(_cBegin));
    cairo_pattern_add_color_stop_rgba(pattern, 1.0, CR_RED(_cEnd), CR_GREEN(_cEnd), CR_BLUE(_cEnd), CR_ALPHA(_cEnd));
    return (pattern);
}

void RectEx::fill(cairo_t* _cr, ColorRef _color) {
    fill(_cr, _color, CR_A(_color));
}

void RectEx::fill(cairo_t* _cr, ColorRef _color, float _alpha) {
    if ((_cr == nullptr) || (_color == 0)) {
        return;
    }
    cairo_set_source_rgba(_cr, CR_R(_color), CR_G(_color), CR_B(_color), std::max(0.0, std::min(1.0, (double)_alpha)));
    cairo_rectangle(_cr, x, y, width, height);
    cairo_fill(_cr);
}

void RectEx::fill(cairo_t* _cr, cairo_pattern_t* _pattern) {
    if ((_cr == nullptr) || (_pattern == nullptr)) {
        return;
    }
    cairo_set_source(_cr, _pattern);
    cairo_rectangle(_cr, x, y, width, height);
    cairo_fill(_cr);
}

void RectEx::center_text(cairo_t* _cr, const char* _string) {
    cairo_save(_cr);

    set(_cr);
    cairo_clip(_cr);

    cairo_text_extents_t extents{ 0 };
    cairo_text_extents(_cr, _string, &extents);

    cairo_move_to(_cr, center_x() - (extents.width / 2), center_y() + (extents.height / 2));
    cairo_show_text(_cr, _string);

    cairo_restore(_cr);
}

bool RectEx::is_pt_in_rect(double _x, double _y) {
    return ((_x >= x1()) && (_x <= x2()) && (_y >= y1()) && (_y <= y2()));
}

void RectEx::draw_frame(cairo_t* _cr, ColorRef _color, float _alpha) {
    const double frameDashes[2] = { 8.0, 4.0 };
    _cairo_set_source_rgb_a(_cr, _color, _alpha);
    cairo_set_antialias(_cr, CAIRO_ANTIALIAS_FAST);
    cairo_set_dash(_cr, frameDashes, 2, 0);
    cairo_set_line_width(_cr, 1.0);
    cairo_rectangle(_cr, x, y, width, height);
    cairo_stroke(_cr);
}

bool RectEx::is_too_small(double minWidth, double minHeight) {
    return (((double)width < minWidth) || ((double)height < minHeight));
}

void RectEx::print(void) {
    printf("x=%d y=%d w=%d h=%d", (int)x, (int)y, (int)width, (int)height);
}

bool RectEx::is_empty(void) {
    return ((x == 0) && (y == 0) && (width == 0) && (height == 0));
}
