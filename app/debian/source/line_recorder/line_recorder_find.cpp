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

void LRFindResult::init(double _x, double _y) {
    clr();
    m.searched_pt.set(_x, _y);
}

void LRFindResult::cleanup(void) {
}

void LRFindResult::set(LRFindResult *source) {
    if (source != nullptr) {
        memcpy(&m, &source->m, sizeof(m));
    }
}

void LRFindResult::clr(void) {
    memset(&m, 0, sizeof(m));
    m.searched_pt.set(-1.0, -1.0);
    m.found_pt.set(-1.0, -1.0);
    m.type = LRElementType::invalid;
    m.subtype = LRElementSub::standard;
    m.delta_px = -1.0;
}

const char* LRFindResult::get_TypeName(LRElementType _type) {
    if (_type == LRElementType::scale)
        return ("scale");
    if (_type == LRElementType::paper)
        return ("paper");
    if (_type == LRElementType::info)
        return ("info");
    return ("invalid");
}

const char* LRFindResult::get_SubTypeName(LRElementSub _subtype) {
    if (_subtype == LRElementSub::standard)
        return ("background");
    if (_subtype == LRElementSub::curve_point)
        return ("curve point");
    if (_subtype == LRElementSub::scale_pointer)
        return ("scale pointer");
    if (_subtype == LRElementSub::info_file)
        return ("info file");
    if (_subtype == LRElementSub::info_wnd)
        return ("info window");
    if (_subtype == LRElementSub::info_sel)
        return ("sel. info");
    return ("invalid");
}

const char* LRFindResult::get_TypeName(void) {
    return (LRFindResult::get_TypeName(m.type));
}

const char* LRFindResult::get_SubTypeName(void) {
    return (LRFindResult::get_SubTypeName(m.subtype));
}

void LRFindResult::set_paper(double _x, double _y, RectEx* _rc_paper) {
    m.type     = LRElementType::paper;
    m.subtype  = LRElementSub::standard;
    m.timecode = Times::get_now();

    m.searched_pt.set(_x, _y);
    m.found_rect.set(_rc_paper);
    m.found_pt.set(_x - _rc_paper->x, _y - _rc_paper->y);
}

void LRFindResult::set_curve_point(EvalCurve* _curve, int _pt_index, PointF* _pt, double _delta_px) {
    m.subtype  = LRElementSub::curve_point;
    m.timecode = _curve->get_timecode(_pt_index);
    m.value    = _curve->get_value(_pt_index);
    m.delta_px = _delta_px;

    m.device.set(_curve->get_device());

    m.scale.set(_curve->get_scale());
    m.scale.set_value(_curve->get_last_value());

    m.found_pt.set(_pt);
    double selecting_range = std::max(4.0, std::min(LR_CAPTURE_THRESHOLD_PX, _delta_px));
    m.found_sub.set(_pt->x - selecting_range, _pt->y - selecting_range, selecting_range * 2.0, selecting_range * 2.0);
}

Scale* LRFindResult::get_scale(void) {
    return (&m.scale);
}

const char* LRFindResult::get_device_serial_number(void) {
    return (m.device.device_serial_number);
}
