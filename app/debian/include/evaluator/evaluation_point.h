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

class EvalPt {
    private:
        double   timecode = 0.0;
        double   interval = TC_MILLISEC;

        uint32_t count    = 0.0f;
        float    sum      = 0.0f;
        float    weight   = 0.0f;
        float    min      = 0.0f;
        float    max      = 0.0f;

    public:
        void clear(void);
        void reset(void);
        void set_center(double _timecode, double _interval);
        bool add_value(double _timecode, float _value);
        double get_timecode(void);
        float get_value(void);
        float get_min(void);
        float get_max(void);
        uint32_t get_count(void);
        float get_precisition(void);
        bool is_used(void);
};
