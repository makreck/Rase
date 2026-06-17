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

void EvaluationSlot::init(int _fd, LogFile* _logfile, LogWindow* _window) {
    m.fd = _fd;
    m.logfile = _logfile;
    m.window.set(_window);
    memset(m.points, 0, sizeof (m.points));

    LogRegistry* reg = m.logfile->get_registry();
    
    if ((reg->get_timecode_end() >= m.window.time.get_begin()) &&
        (reg->get_timecode_begin() <= m.window.time.get_end())) {
        perform_async();
    } else {
        m.data_ready = true;
    }
}

void EvaluationSlot::cleanup() {
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
}

EvalPt* EvaluationSlot::create_points(void) {
    size_t size = sizeof (EvalPt) * LOG_EVAL_CURVE_LEN_MAX;
    EvalPt* channel = (EvalPt*)malloc(size);
    if (channel != nullptr) {
        double time_begin = m.window.time.get_begin();
        double time_span  = m.window.time.get_span();
        double time_interval = time_span / (double)LOG_EVAL_CURVE_LEN_MAX;
        for (int i = 0; i < LOG_EVAL_CURVE_LEN_MAX; i++) {
            double time_center = ((double)i * time_span / (double)LOG_EVAL_CURVE_LEN_MAX) + time_begin;
            channel[i].set_center(time_center, time_interval);
        }
    }
    return (channel);
}

EvalPt* EvaluationSlot::get_points(int _index, bool auto_create) {
    if ((_index < 0) || (_index >= LOG_SLOT_MAX)) {
        return (nullptr);
    }

    if ((m.points[_index] == nullptr) && auto_create) {
        m.points[_index] = create_points();
    }

    return (m.points[_index]);
}

LogWindow* EvaluationSlot::get_window(void) {
    return (&m.window);
}

bool EvaluationSlot::is_data_ready(void) {
    if (m.data_ready) {
        if (m.thread_handle != INVALID_THREAD_HANDLE) {
            pthread_join(m.thread_handle, nullptr);
            m.thread_handle = INVALID_THREAD_HANDLE;
        }
    }
    return (m.data_ready);
}

bool EvaluationSlot::wait_for_data_ready(void) {
    if (m.thread_handle != INVALID_THREAD_HANDLE) {
        pthread_join(m.thread_handle, nullptr);
        m.thread_handle = INVALID_THREAD_HANDLE;
    }
    return (m.data_ready);
}

void EvaluationSlot::perform_async(void) {
    m.data_ready = false;
    pthread_create(&m.thread_handle, nullptr, EvaluationSlot::_evaluation_thread, this);
}

void* EvaluationSlot::_evaluation_thread(void *_object) {
    (reinterpret_cast<EvaluationSlot *>(_object))->evaluation_thread();
    return (nullptr);
}
void EvaluationSlot::evaluation_thread(void) {
    perform_sync();
}

void EvaluationSlot::perform_sync(void) {
    int64_t scan_position = m.logfile->get_registry()->get_file_position_for(m.window.time.begin);
    LogFrame frame;
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
    m.data_ready = true;
}
