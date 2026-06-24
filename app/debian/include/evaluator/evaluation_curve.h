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
        double  timecode;
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
    public:
        Scale        scale;
        int          slot   = 0;
        size_t       length = 0;
        EvalCurvePt* data   = nullptr;

        EvalCurve(size_t _length, int _slot, Scale* _scale) {
            init(_length, _slot, _scale);
        }

        ~EvalCurve() {
            cleanup();
        }

        void init(size_t _length, int _slot, Scale* _scale);
        void cleanup(void);
        void draw(cairo_t* _cr, RectEx& _rc, bool _foreground_curve = false);
        void draw_stopper(cairo_t *_cr, RectEx& _rc, double y);
        int  get_slot(void);
        size_t get_length(void);
        PointF* get_point(int _index);
        double get_timecode(int _index);
        bool set(int _index, double _timecode, double _x, double _y);
        bool is_used(int _index);
        bool is_begin(int _index);
        bool is_end(int _index);
        bool is_single(int _index);
        bool set_begin(int _index, bool state = true);
        bool set_end(int _index, bool state = true);
        bool set_symbol(int _index, uint8_t _symbol = 0x00);
};
