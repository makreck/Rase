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

class LogRange {
    public:
        double begin = 0.0;
        double end = 1.0;

        void operator=(const LogRange* _source);
        void operator=(const LogRange& _source);
        bool operator==(const LogRange& _source);

        void set(long double _given_begin, long double _given_end);
        void set(LogRange& _given);
        void set(LogRange* _given);
        double get_span(void);
        double get_begin(void);
        double get_end(void);
        void set_begin(double _given_begin);
        void set_end(double _given_end);
        void add_offset(double _givenOffset);
        double approximate(double _timecode, double _threshold = (25.0 * TC_MILLISEC), double _factor = 0.25);
        double move_to(void);
        double move_to(double _given_end);
        void extend(double _factor);
        void center(double _pivot, double _range);
        bool set_when_threshold_is_exceeded(LogRange& _source, double _threshold);
        double center(void);
        bool is_equal(LogRange* _givenRange, double _threshold = 0.0);
        bool is_in_window(double _value);
        bool is_overlapping(const LogRange* _source);
};
