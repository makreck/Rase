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

void EvaluationTask::init(Evaluator* _base, int _task_index, int _fd) {
    memset(m.points, 0, sizeof (m.points));
    m.base          = _base;
    m.task_index    = _task_index;
    m.fd            = _fd;
    m.data_ready    = true;
    m.logfile       = new LogFile();
    pthread_create(&m.thread_handle, nullptr, EvaluationTask::_evaluation_thread, this);
}

void EvaluationTask::cleanup() {
    m.fd = -1;

    if (m.thread_handle != INVALID_THREAD_HANDLE) {
        pthread_cancel(m.thread_handle);
        pthread_join(m.thread_handle, nullptr);
        m.thread_handle = INVALID_THREAD_HANDLE;
    }

    m.data_ready = false;

    for (int i = 0; i < LOG_SLOT_MAX; i++) {
        if (m.points[i] != nullptr) {
            free(m.points[i]);
            m.points[i] = nullptr;
        }
    }

    if (m.logfile != nullptr) {
        delete (m.logfile);
    }
}

void EvaluationTask::reset_points(EvalPt* _points) {
    if (_points != nullptr) {
        double time_begin = m.window.time.get_begin();
        double time_span  = m.window.time.get_span();
        double time_interval = time_span / (double)LOG_EVAL_CURVE_LEN_MAX;
        for (int i = 0; i < LOG_EVAL_CURVE_LEN_MAX; i++) {
            double time_center = ((double)i * time_span / (double)LOG_EVAL_CURVE_LEN_MAX) + time_begin;
            _points[i].set_center(time_center, time_interval);
        }
    }
}

void EvaluationTask::init_points(void) {
    for (int i = 0; i < LOG_SLOT_MAX; i++) {
        if (m.points[i] != nullptr) {
            reset_points(m.points[i]);
        }
    }
}

EvalPt* EvaluationTask::get_points(int _index, bool _auto_create) {
    if ((_index < 0) || (_index >= LOG_SLOT_MAX)) {
        return (nullptr);
    }

    if ((m.points[_index] == nullptr) && _auto_create) {
        m.points[_index] = (EvalPt*)malloc(sizeof (EvalPt) * LOG_EVAL_CURVE_LEN_MAX);
        reset_points(m.points[_index]);
    }

    return (m.points[_index]);
}

LogWindow* EvaluationTask::get_window(void) {
    return (&m.window);
}

bool EvaluationTask::is_data_ready(void) {
    return (m.data_ready);
}

Scale* EvaluationTask::get_scale(int _index) {
    if (m.logfile != nullptr) {
        LogInventory* inventory = m.logfile->get_inventory();
        if (inventory != nullptr) {
            return (inventory->get_slot(_index));
        }
    }
    return (nullptr);
}

void* EvaluationTask::_evaluation_thread(void *_object) {
    (reinterpret_cast<EvaluationTask *>(_object))->evaluation_thread();
    return (nullptr);
}
void EvaluationTask::evaluation_thread(void) {
    while (true) {
        if ((m.data_ready == true) || (m.fd == -1)) {
            usleep(1000);
            continue;
        }
        init_points();
        scan();
        set_ready();
    }
}

void EvaluationTask::scan(void) {
    LogFrame frame;
    int64_t scan_position = m.logfile->get_registry()->get_file_position_for(m.window.time.begin);
    while (m.logfile->get_frame(m.fd, scan_position, &frame)) {
        scan_position += sizeof (LogFrame);

        int slot_index = frame.get_slot();
        EvalPt* points = get_points(slot_index, true);
        if (points == nullptr) { continue; }

        double timecode = frame.get_timecode();
        if (timecode < m.window.time.begin) { continue; }
        if (timecode > m.window.time.end)   { break;    }

        int pt_index = (int)(((timecode - m.window.time.begin) * (double)LOG_EVAL_CURVE_LEN_MAX / m.window.time.get_span()) + 0.5);
        if ((pt_index < 0) || (pt_index >= LOG_EVAL_CURVE_LEN_MAX)) { continue; }
        
        if (pt_index > 0) {
            points[pt_index - 1].add_value(timecode, frame.get_value());
        }

        points[pt_index].add_value(timecode, frame.get_value());

        if (pt_index < (LOG_EVAL_CURVE_LEN_MAX - 1)) {
            points[pt_index + 1].add_value(timecode, frame.get_value());
        }
    }
}

void EvaluationTask::set_window(LogWindow* _window) {
    if (m.data_ready == true) {
        m.window.set(_window);
        m.logfile->get(m.fd);
        LogRegistry* reg = m.logfile->get_registry();
        if ((reg->get_timecode_end() >= m.window.time.get_begin()) &&
            (reg->get_timecode_begin() <= m.window.time.get_end())) {
            m.data_ready = false;
        } else {
            init_points();
            set_ready();
        }
    }
}

void EvaluationTask::set_ready(void) {
    m.data_ready = true;
    if (m.base != nullptr) {
        m.base->set_active(m.task_index);
    }
}
