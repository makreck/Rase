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

#include "evaluator/evaluation_defs.h"
#include "evaluator/evaluation_point.h"
#include "evaluator/evaluation_slot.h"
#include "evaluator/evaluation_curve.h"

class Evaluator {
    private:
        struct {
            std::string path;
            int fd = -1;
            LogFile* logfile = nullptr;
            LogWindow window;
            LogWindow ext_window;
            
            pthread_mutex_t slot_mutex = PTHREAD_MUTEX_INITIALIZER;

            int active_slot = 0;
            EvaluationSlot* evaluation_slot[2]{ nullptr };

            std::vector<EvalCurve*> curve_list;
        } m;

        void init(const char* _path);
        void cleanup(void);

    public:
        Evaluator(const char* _path) {
            init(_path);
        }
        
        ~Evaluator() {
            cleanup();
        }

        void delete_curve_list(void);
        const char* get_path(void);
        void set_window(LogWindow _window);
        void draw_curves(cairo_t* cr, RectEx& _rect, LogWindow _window, bool _vertical);
};
