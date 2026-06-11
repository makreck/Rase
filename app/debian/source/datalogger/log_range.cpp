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

void LogRange::operator=(const LogRange* _source) {
    begin = _source->begin;
    end = _source->end;
}

void LogRange::operator=(const LogRange& _source) {
    begin = _source.begin;
    end = _source.end;
}

bool LogRange::operator==(const LogRange& _source) {
    return ((begin == _source.begin) && (end == _source.end));
}

void LogRange::set(long double _given_begin, long double _given_end) {
    begin = (double)_given_begin;
    end = (double)_given_end;
}

void LogRange::set(LogRange& _given) {
    begin = _given.get_begin();
    end = _given.get_end();
}

void LogRange::set(LogRange* _given) {
    if (_given != nullptr) {
        begin = _given->get_begin();
        end = _given->get_end();
    } else {
        begin = 0.0;
        end = 1.0;
    }
}

double LogRange::get_span(void) {
    return (end - begin);
}

double LogRange::get_begin(void) {
    return (begin);
}

double LogRange::get_end(void) {
    return (end);
}

void LogRange::set_begin(double _given_begin) {
    begin = _given_begin;
}

void LogRange::set_end(double _given_end) {
    end = _given_end;
}

void LogRange::add_offset(double givenOffset) {
    begin += givenOffset;
    end += givenOffset;
}

double LogRange::move_to(double _given_end) {
    double range = end - begin;
    double moved = end = _given_end;
    begin = end - range;
    return (moved);
}

double LogRange::move_to(void) {
    double now = Times::get_now();
    double moved = now - end;
    double range = end - begin;
    end = now;
    begin = end - range;
    return (moved);
}

double LogRange::approximate(double _timecode, double _threshold, double _factor) {
    _factor = std::max(0.001, std::min(1.0, fabs(_factor)));
    double moved = (_timecode - end) * _factor;
    if ((_threshold != 0.0) && (fabs(moved) < fabs(_threshold))) {
        moved = _timecode - end;
    }
    double range = end - begin;
    end += moved;
    begin = end - range;
    return (moved);
}

void LogRange::extend(double _factor) {
    begin -= _factor;
    end += _factor;
}

void LogRange::center(double _pivot, double _range) {
    begin = _pivot - (_range / 2.0);
    end = _pivot + (_range / 2.0);
}

bool LogRange::set_when_threshold_is_exceeded(LogRange& _source, double _threshold) {
    if ((fabs(_source.begin - begin) > _threshold) ||
        (fabs(_source.end - end) > _threshold)) {
        set(_source);
        return (true);
    }

    return (false);
}

double LogRange::center(void) {
    return ((begin + end) / 2.0);
}

bool LogRange::is_equal(LogRange* _given_range, double _threshold) {
    if (_given_range == nullptr) {
        return (false);
    }
    if (_threshold == NAN) {
        _threshold = 0.0;
    } else {
        _threshold = fabs(_threshold);
    }
    return (((fabs(_given_range->begin - begin)) <= _threshold) && ((fabs(_given_range->end - end)) <= _threshold));
}

bool LogRange::is_in_window(double value) {
    return ((value >= begin) && (value <= end));
}
