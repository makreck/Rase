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
        struct {
            PointF pt;
            double timecode;
            float  value;
            union {
                uint32_t flags;
                struct {
                    uint32_t symbol       : 8;
                    uint32_t f_used       : 1;
                    uint32_t f_startpoint : 1;
                    uint32_t f_endpoint   : 1;
                    uint32_t reserved1    : 5;
                    uint32_t reserved2    : 8;
                    uint32_t reserved3    : 8;
                };
            };
        } m;

        void set(EvalCurvePt* _source) {
            if (_source != nullptr) {
                memcpy(&m, &_source->m, sizeof (m));
            } else {
                memset(&m, 0, sizeof (m));
            }
        }
};

class EvalCurve {
    public:
        ProductID device;
        Scale     scale;
        union {
            uint32_t flags;
            struct {
                uint32_t length     : 16;
                uint32_t slot       : 4;
                uint32_t f_selected : 1;
                uint32_t reserved2  : 3;
                uint32_t reserved3  : 8;
            };
        };

        EvalCurvePt* data = nullptr;

        EvalCurve(size_t _length, int _slot, ProductID* _product_id, Scale* _scale) {
            init(_length, _slot, _product_id, _scale);
        }

        ~EvalCurve() {
            cleanup();
        }

        void init(size_t _length, int _slot, ProductID* _product_id, Scale* _scale);
        void cleanup(void);
        void draw(cairo_t* _cr, RectEx& _rc, bool _foreground_curve = false);
        void draw_stopper(cairo_t *_cr, RectEx& _rc, double y);
        int  get_slot(void);
        size_t get_length(void);
        PointF* get_point(int _index);
        Scale* get_scale(void);
        ProductID* get_device(void);
        double get_timecode(int _index);
        float get_value(int _index);
        float get_newest_value(void);
        void set_selected(bool _state = true);
        bool set(int _index, double _timecode, float _value, float _x, float _y);
        bool is_selected(void);
        bool is_used(int _index);
        bool is_begin(int _index);
        bool is_end(int _index);
        bool is_single(int _index);
        bool set_begin(int _index, bool state = true);
        bool set_end(int _index, bool state = true);
        bool set_symbol(int _index, uint8_t _symbol = 0x00);
        bool clean_curve(void);

};
