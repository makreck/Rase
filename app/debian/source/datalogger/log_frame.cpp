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

bool LogFrame::set(const LogFrame &_source) {
    return (set(&_source));
}

bool LogFrame::set(const LogFrame *_source) {
    if (_source == nullptr) {
        return (false);
    }
    memcpy(buffer, _source, sizeof(buffer));
    return (true);
}

void LogFrame::clear(void) {
    memset(buffer, 0, sizeof(buffer));
    magic_id = (uint32_t)LOG_FRAME_MAGIC_BYTE;
}

double LogFrame::get_timecode(void) {
    return (timecode);
}

float LogFrame::get_value(void) {
    return (value);
}

int LogFrame::get_slot(void) {
    return ((int)slot);
}

int LogFrame::get_symbol(void) {
    return ((int)symbol);
}

bool LogFrame::set_timecode(double _timecode) {
    if (__isnan(_timecode)) {
        return (false);
    }
    if (_timecode > 0.0) {
        timecode = _timecode;
    } else {
        Times::get_timestamp_local();
    }
    return (true);
}

bool LogFrame::set_value(float _value) {
    if (__isnan(_value)) {
        return (false);
    }
    value = _value;
    return (true);
}

bool LogFrame::set_slot(int _slot) {
    if (_slot != (_slot & 0x0f)) {
        return (false);
    }
    slot = (uint32_t)(_slot & 0x0f);
    return (true);
}

bool LogFrame::set_symbol(int _symbol) {
    if (_symbol != (_symbol & 0x0f)) {
        return (false);
    }
    symbol = (uint32_t)(_symbol & 0x0f);
    return (true);
}

bool LogFrame::get(LogFrame& _target) {
    return (get(&_target));
}

bool LogFrame::get(LogFrame* _target) {
    if (_target == nullptr) {
        return (false);
    }
    memcpy(_target, buffer, sizeof (buffer));
    return (true); 
}

