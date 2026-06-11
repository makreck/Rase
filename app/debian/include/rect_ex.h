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

enum class RectDir {
    horizontal = 0,
    vertical = 1,
};

class RectEx : public GdkRectangle {
    public:
        RectEx() = default;

        RectEx(double _x, double _y, double _width, double _height) {
            set(_x, _y, _width, _height);
        }

        RectEx(RectEx& source) {
            set(source);
        }

        RectEx(RectEx* source) {
            set(source);
        }

        RectEx(GtkWidget* widget) {
            set(widget);
        }

        void operator=(const GdkRectangle& src);
        void operator=(const RectEx* src);
        void operator=(const RectEx& src);
        bool operator==(const RectEx& src);
        bool operator!=(const RectEx& src);

        static double format_string(const char* string);

        void set(RectEx& source);
        void set(RectEx* source);
        void set(double _x, double _y, double _width, double _height);
        void set(GtkWidget* widget);
        void set(PointF* pt, double _width = 1.0, double _height = 1.0);
        void set(cairo_t* hdc);
        void normalize(void);
        void clr(void);
        void set_left(const char* string);
        void set_top(const char* string);
        void set_right(const char* string);
        void set_bottom(const char* string);
        void shrink_width(double n);
        void shrink_height(double n);
        void shrink(double n);
        void expand_width(double n);
        void expand_height(double n);
        void expand(double n);
        void offset(double _x, double _y);
        void clip(cairo_t *hdc);
        void print(void);

        int x1(void);
        int y1(void);
        int x2(void);
        int y2(void);
        int center_x(void);
        int center_y(void);

        cairo_pattern_t* linear_gradient(RectDir direction, ColorRef cBegin, ColorRef cEnd);

        void fill(cairo_t* hdc, ColorRef color);
        void fill(cairo_t *hdc, ColorRef color, float alpha);
        void fill(cairo_t *hdc, cairo_pattern_t* pattern);
        void center_text(cairo_t *hdc, const char* string);
        void draw_frame(cairo_t *hdc, ColorRef color, float alpha = 1.0f);

        bool is_pt_in_rect(double x, double y);
        bool is_too_small(double minWidth, double minHeight);
        bool is_empty(void);
};
