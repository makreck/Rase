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

bool LineRecorder::enable_evaluation(const char* _path) {
    for (Evaluator*& evaluator : m.curves) {
        if (evaluator != nullptr) {
            if (strcmp(_path, evaluator->get_path()) == 0) {
                // @todo: Set recording active ...
                return (true);
            }
        }
    }
    m.curves.push_back(new Evaluator(_path));
    return (true);
}

bool LineRecorder::disable_evaluation(const char* _path) {
    for (Evaluator*& evaluator : m.curves) {
        if (evaluator != nullptr) {
            if (strcmp(_path, evaluator->get_path()) == 0) {
                // @todo: Set recording inactive ...
                return (true);
            }
        }
    }
    return (false);
}
