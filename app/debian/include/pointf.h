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

class PointF {
    public:
        union {
            struct {
                float x;
                float y;
            };
            struct {
                float pos;
                float value;
            };
        };

        PointF(float givenX = 0.0f, float givenY = 0.0f) {
            x = givenX;
            y = givenY;
        }

        void set(double _x, double _y) {
            x = (float)(_x);
            y = (float)(_y);
        }

        void set(PointF& source) {
            x = source.x;
            y = source.y;
        }

        void set(PointF* _source) {
            if (_source != nullptr) {
                x = _source->x;
                y = _source->y;
            }
        }

        bool equals(PointF* _the_other_point) {
            if (_the_other_point != nullptr) {
                return ((_the_other_point->x == x) && (_the_other_point->y == y));
            }
            return (false);
        }

        bool equals(PointF& _the_other_point) {
            return ((_the_other_point.x == x) && (_the_other_point.y == y));
        }
};
