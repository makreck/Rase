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

enum class ScaleLayout {
    normal_horizontal = 0,
    normal_vertical   = 1,
    normal_radial     = 2,
};

enum class ScalePointerType {
    none    = 0,
    pointer = 1,
    line    = 2,
    bar     = 3,
    fill    = 4,
};

class ScaleStep {
    public:
        double value = 0.0;
        double pos = 0.0;
        union {
            uint64_t flag = 0;
            struct {
                uint64_t f_main_divider : 1;
                uint64_t f_half_divider : 1;
                uint64_t fReserved0 : 6;
                uint64_t fReserved1 : 8;
                uint64_t fReserved2 : 8;
                uint64_t fReserved3 : 8;
                uint64_t fReserved4 : 8;
                uint64_t fReserved5 : 8;
                uint64_t fReserved6 : 8;
                uint64_t fReserved7 : 8;
            };
        };
};

class ScaleDrawing : public Scale {
    private:
        bool update_request = true;

        void calculate_for(ScaleLayout layout, RectEx &rc, int maxTextWidth_px, std::vector<ScaleStep> &dividers);
        void draw_line_pointer_horizontal(cairo_t* cr, PointF &pointer, RectEx &rc, ColorRef color_pointer);
        void draw_bar_pointer_horizontal(cairo_t* cr, PointF &pointer, RectEx &rc, ColorRef color_pointer);
        void draw_fill_pointer_horizontal(cairo_t* cr, PointF &pointer, RectEx &rc, ColorRef color_pointer);
        void draw_regular_pointer_horizontal(cairo_t* cr, PointF &pointer, RectEx &rc, ColorRef color_pointer);
        void draw_scale_horizontal(ScalePointerType type, cairo_t* cr, RectEx* rc_area, RectEx* rc_scale,
            ColorRef color_background, ColorRef color_foreground, ColorRef color_pointer, const char* headline, std::vector<ScaleStep> &dividers);

    public:
        void draw(ScaleLayout layout, ScalePointerType type, cairo_t* cr, RectEx *rc_area, RectEx *rc_scale,
            ColorRef color_background, ColorRef color_foreground, ColorRef color_pointer, const char *headline, std::vector<ScaleStep> &dividers);

        size_t format_value(const ScaleFormat& _used_format, char* _buffer, size_t _size, double _value, int _stride, bool _with_unit);
        size_t format_value(char* _buffer, size_t _size, double _value, bool _with_unit);
        size_t format_value(char* _buffer, size_t _size, bool _with_unit);

        bool get_update_request(void);
        void set_update_request(bool _state = true);
        void clr_update_request(void);
};

