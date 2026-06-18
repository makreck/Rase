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

class Evaluator;

class EvaluationTask {
    private:
        struct {
            Evaluator*    base = nullptr;
            int           task_index = 0;
            LogFile*      logfile = nullptr;
            LogWindow     window;
            EvalPt*       points[LOG_SLOT_MAX];
            pthread_t     thread_handle = INVALID_THREAD_HANDLE;
            volatile bool data_ready = false;
        } m;

        static void* _evaluation_thread(void* _object);
        void evaluation_thread(void);

        void init(Evaluator* _base, int _task_index);
        void cleanup(void);
        void reset_points(EvalPt* _points);
        void init_points(void);
        void set_ready(void);
        void scan(void);

    public:
        EvaluationTask(Evaluator* _base, int _task_index) {
            init(_base, _task_index);
        }

        ~EvaluationTask() {
            cleanup();
        }

        LogWindow* get_window(void);
        EvalPt* get_points(int _index, bool _auto_create);
        bool is_data_ready(void);
        Scale* get_scale(int _index);
        void set_window(LogWindow* _window);

};
