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
        union {
            uint32_t flags;
            struct {
                uint32_t f_startpoint : 1;
                uint32_t f_endpoint   : 1;
                uint32_t f_reserved0  : 6;
                uint32_t symbol       : 8;
                uint32_t reserved2    : 8;
                uint32_t reserved3    : 8;
            };
        };
};

class EvalCurve {
    private:
        void draw_stopper(cairo_t *_cr, double y);

    public:
        int          index      = 0;
        ColorRef     color      = C_RED;
        float        line_width = 2.0f;
        size_t       length     = 0;
        EvalCurvePt* data       = nullptr;
        RectEx       rc;

        EvalCurve(size_t _length, int _index, ColorRef _color, float _line_width, RectEx& _rect) {
            init(_length, _index, _color, _line_width, _rect);
        }

        ~EvalCurve() {
            cleanup();
        }

        void init(size_t _length, int _index, ColorRef _color, float _line_width, RectEx& _rect);
        void cleanup(void);
        bool set(int _index, double _x, double _y);
        bool set_property(int _index, uint8_t _symbol, bool _startpoint, bool _endpoint);
        void draw(cairo_t* _cr, bool _foreground_curve = false);
};
