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

void EvalPt::clear(void) {
    timecode = 0.0;
    interval = TC_MILLISEC;
    reset();
}

void EvalPt::reset(void) {
    sum = 0.0f;
    weight = 0.0f;
    min = 0.0f;
    max = 0.0f;
    count = 0;
}

void EvalPt::set_center(double _timecode, double _interval) {
    timecode = _timecode;
    interval = std::max(TC_MILLISEC, _interval);
    reset();
}

bool EvalPt::add_value(double _timecode, float _value) {
    if (__isnan(_value)) {
        return (false);
    }

    double delta = fabs(_timecode - timecode);
    if (delta >= (interval * 0.5)) {
        return (false);
    }

    if (count != 0) {
        min = std::min(min, _value);
        max = std::max(max, _value);
    } else {
        min = _value;
        max = _value;
    }

    double tc_weight = 1.0 - (delta / (interval * 0.5));
    sum += ((double)_value * tc_weight);
    weight += tc_weight;

    count++;

    return (true);
}

double EvalPt::get_timecode(void) {
    return (timecode);
}

float EvalPt::get_value(void) {
    if (weight != 0.0f) {
        return (sum / weight);
    }
    return (0.0f);
}

float EvalPt::get_min(void) {
    return (min);
}

float EvalPt::get_max(void) {
    return (max);
}

uint32_t EvalPt::get_count(void) {
    return (count);
}

float EvalPt::get_precisition(void) {
    if (count == 0) return (-1.0);
    return (weight / (float)count);
}

bool EvalPt::is_used(void) {
    return (count != 0);
}
