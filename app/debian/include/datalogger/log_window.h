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

class LogWindow {
    public:
        LogRange time;
        LogRange level;

        LogWindow() {
            time.set(0.0, 1.0);
            level.set(0.0, 1.0);
        };

        LogWindow(LogWindow* _source) {
            set(_source);
        };

        LogWindow(LogWindow& _source) {
            set(_source);
        };

        void set(LogWindow& givenWindow);
        void set(LogWindow* givenWindow);
        void set(long double timeBegin, long double timeEnd, long double levelBegin, long double levelEnd);
        void set_time_endpoint_and_begin_by_offset(double _given_end, double givenOffset);
        void set_begin_and_span_by_offset(double _given_begin, double offset);
        void shift_time(double offset);
        void shift_level(double offset);
        void center_time(double pivot, double range);
        void center_level(double pivot, double range);
        double get_time_center(void);
        double get_level_center(void);
        bool is_visible(double givenTimeCode);
        bool is_equal(LogWindow* givenWindow);
        bool is_equal_in_time(LogWindow* givenWindow);
        bool is_time_in_window(double timecode);
        void expand(double _factor);
};
