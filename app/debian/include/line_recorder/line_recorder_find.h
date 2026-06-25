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

#define LR_CAPTURE_THRESHOLD_PX         (10.0)

enum class LRElementType {
    invalid = 0,
    scale   = 1,
    paper   = 2,
    info    = 3,
};

enum class LRElementSub {
    standard      = 0,
    scale_pointer = 10,
    curve_point   = 20,
    info_file     = 30,
    info_wnd      = 31,
    info_sel      = 32,
};

class LRFindResult {
    private:
        void init(double _x, double _y);
        void cleanup(void);

    public:
        struct {
            LRElementType type;
            LRElementSub  subtype;
            ProductID     device;
            Scale         scale;
            PointF        searched_pt;  
            PointF        found_pt;
            RectEx        found_rect;
            RectEx        found_sub;
            double        timecode;
            double        value;
            double        delta_px;
        } m;

        LRFindResult(double _x = 0.0, double _y = 0.0) {
            init(_x, _y);
        }

        ~LRFindResult() {
            cleanup();
        }

        static const char* get_TypeName(LRElementType _type);
        static const char* get_SubTypeName(LRElementSub _subtype);

        const char* get_TypeName(void);
        const char* get_SubTypeName(void);

        void set(LRFindResult* source);
        void set_curve_point(EvalCurve* _curve, int _pt_index, PointF* _pt, double _delta_px);
        void set_paper(double _x, double _y, RectEx* _rc_paper);
        void clr(void);
        Scale* get_scale(void);

};


