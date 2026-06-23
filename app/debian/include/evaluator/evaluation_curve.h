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

class EvalCurvePt {
    public:
        PointF  pt;
        uint8_t symbol;
        union {
            uint8_t flags;
            struct {
                uint8_t f_used       : 1;
                uint8_t f_startpoint : 1;
                uint8_t f_endpoint   : 1;
                uint8_t f_reserved0  : 5;
            };
        };
};

class EvalCurve {
    private:
        void draw_stopper(cairo_t *_cr, double y);

    public:
        int          slot       = 0;
        size_t       length     = 0;
        float        line_width = 2.0f;
        ColorRef     color      = C_RED;
        RectEx       rc;
        EvalCurvePt* data       = nullptr;

        EvalCurve(size_t _length, int _slot, ColorRef _color, float _line_width, RectEx& _rect) {
            init(_length, _slot, _color, _line_width, _rect);
        }

        ~EvalCurve() {
            cleanup();
        }

        void init(size_t _length, int _slot, ColorRef _color, float _line_width, RectEx& _rect);
        void cleanup(void);
        void draw(cairo_t* _cr, bool _foreground_curve = false);
        int  get_slot(void);
        bool set(int _index, double _x, double _y);
        bool is_used(int _index);
        bool is_begin(int _index);
        bool is_end(int _index);
        bool is_single(int _index);
        bool set_begin(int _index, bool state = true);
        bool set_end(int _index, bool state = true);
        bool set_symbol(int _index, uint8_t _symbol = 0x00);
};
