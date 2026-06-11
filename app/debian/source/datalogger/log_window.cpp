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

void LogWindow::set(LogWindow& givenWindow) {
    set(&givenWindow);
}

void LogWindow::set(LogWindow* givenWindow) {
    if (givenWindow != nullptr) {
        time.set(givenWindow->time);
        level.set(givenWindow->level);
    }
}

void LogWindow::set(long double timeBegin, long double timeEnd, long double levelBegin, long double levelEnd) {
    time.set(timeBegin, timeEnd);
    level.set(levelBegin, levelEnd);
}

void LogWindow::set_time_endpoint_and_begin_by_offset(double givenEnd, double givenOffset) {
    time.end = givenEnd;
    time.begin = time.end - givenOffset;
}

void LogWindow::set_begin_and_span_by_offset(double givenBegin, double offset) {
    time.begin = givenBegin;
    time.end = time.begin + offset;
}

void LogWindow::shift_time(double offset) {
    time.add_offset(offset);
}

void LogWindow::shift_level(double offset) {
    level.add_offset(offset);
}

void LogWindow::center_time(double pivot, double range) {
    time.center(pivot, range);
}

void LogWindow::center_level(double pivot, double range) {
    level.center(pivot, range);
}

double LogWindow::get_time_center(void) {
    return (time.center());
}

double LogWindow::get_level_center(void) {
    return (level.center());
}

bool LogWindow::is_visible(double givenTimeCode) {
    return ((givenTimeCode >= time.begin) && (givenTimeCode <= time.end));
}

bool LogWindow::is_equal(LogWindow* givenWindow) {
    if (givenWindow == nullptr) {
        return (false);
    }
    return (time.is_equal(&givenWindow->time, TC_MILLISEC * 0.9) && level.is_equal(&givenWindow->level, 0.0));
}

bool LogWindow::is_equal_in_time(LogWindow* givenWindow) {
    if (givenWindow == nullptr) {
        return (false);
    }
    return (time.is_equal(&givenWindow->time, TC_MILLISEC * 0.9));
}

bool LogWindow::is_time_in_window(double timecode) {
    return (time.is_in_window(timecode));
}

void LogWindow::expand(double _factor) {
    double offset = fabs(time.get_span() * _factor);
    time.begin -= offset;
    time.end   += offset;
}
