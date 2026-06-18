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

void Evaluator::init(const char* _path) {
    m.path = _path;
    Files::open_file(m.fd, m.path.c_str(), O_RDWR);
    pthread_mutex_init(&m.slot_mutex, nullptr);
    for (int i = 0; i < SIZEOFARRAY(m.evaluation_task); i++) {
        m.evaluation_task[i] = new EvaluationTask(this, i);
    }
}

void Evaluator::cleanup(void) {
    pthread_mutex_lock(&m.slot_mutex); {
        delete_curve_list();

        for (int i = 0; i < SIZEOFARRAY(m.evaluation_task); i++) {
            if (m.evaluation_task[i] != nullptr) {
                delete (m.evaluation_task[i]);
                m.evaluation_task[i] = nullptr;
            }
        }

        if (m.fd != -1) {
            Files::close_file(m.fd);
        }

    } pthread_mutex_unlock(&m.slot_mutex);

    pthread_mutex_destroy(&m.slot_mutex);
}

const char* Evaluator::get_path(void) {
    return (m.path.c_str());
}

int Evaluator::get_fd(void) {
    return (m.fd);
}

void Evaluator::delete_curve_list(void) {
    if (m.curve_list.size() > 0) {
        for (EvalCurve*& curve : m.curve_list) {
            if (curve != nullptr) {
                delete (curve);
                curve = nullptr;
            }
        }
        m.curve_list.clear();
    }
}

void Evaluator::draw_curves(cairo_t* _cr, RectEx& _rect, LogWindow& _window, bool _vertical) {
    if ((m.active_task < 0) || (m.active_task >= SIZEOFARRAY(m.evaluation_task))) {
        return;
    }

    pthread_mutex_lock(&m.slot_mutex); {
        EvaluationTask *task = m.evaluation_task[m.active_task];
        delete_curve_list();

        if (task != nullptr) {
            for (int channel_index = 0; channel_index < LOG_SLOT_MAX; channel_index++) {

                LogWindow *slot_window = task->get_window();
                if (!_window.time.is_overlapping(&slot_window->time)) {
                    continue;
                }

                EvalPt *eval_point = task->get_points(channel_index, false);
                if (eval_point == nullptr) {
                    continue;
                }

                Scale *scale = task->get_scale(channel_index);
                if (scale == nullptr) {
                    continue;
                }

                int count = 0;
                for (int i = 0; i < LOG_EVAL_CURVE_LEN_MAX; i++) {
                    if (eval_point[i].is_used()) {
                        count++;
                    }
                }

                if (count > 0) {
                    EvalCurve *curve = new EvalCurve(count, channel_index, scale->get_color_ref(), scale->get_line_width(), _rect);
                    if (curve == nullptr) {
                        continue;
                    }

                    double t_begin = _window.time.get_begin();
                    double t_end = _window.time.get_end();
                    double t_span = _window.time.get_span();
                    double v_begin = 0.0; // _window.level.get_begin();
                    double v_span = 1.0;  // _window.level.get_span();

                    int gap = 0;
                    int n = -1;
                    for (int i = 0; (i < LOG_EVAL_CURVE_LEN_MAX) && (n < count); i++) {
                        if (eval_point[i].is_used()) {
                            float norm_value = scale->get_zoom_normalized(eval_point[i].get_value());
                            double timecode = eval_point[i].get_timecode();

                            if ((gap >= LOG_EVAL_MAX_GAP) && (n >= 0)) {
                                curve->set_property(n, 0x00, false, true);
                            }

                            if (_vertical) {
                                curve->set(++n, ((norm_value - v_begin) * (double)_rect.width / v_span) + (double)_rect.x,
                                           ((t_end - timecode) * (double)_rect.height / t_span) + (double)_rect.y);
                            } else {
                                curve->set(++n, ((timecode - t_begin) * (double)_rect.width / t_span) + (double)_rect.x,
                                           ((norm_value - v_begin) * (double)_rect.height / v_span) + (double)_rect.y);
                            }

                            curve->set_property(n, 0x00, ((gap >= LOG_EVAL_MAX_GAP) || (n == 0)), false);
                            gap = 0;
                        } else {
                            gap++;
                        }
                    }

                    m.curve_list.push_back(curve);
                }
            }
        }

        for (EvalCurve *&curve : m.curve_list) {
            curve->draw(_cr);
        }
    } pthread_mutex_unlock(&m.slot_mutex);
}

void Evaluator::set_window(LogWindow _window) {
    if (m.fd == -1) {
        return;
    }

    LogWindow ext_window = _window;
    ext_window.expand(LOG_DISPLAY_WINDOW_EXPAND_FACTOR);
    int next_task = (m.active_task + 1) % (int)SIZEOFARRAY(m.evaluation_task);
    m.evaluation_task[next_task]->set_window(&ext_window);
}

void Evaluator::set_active(int _task_index) {
    if ((_task_index >= 0) && (_task_index < SIZEOFARRAY(m.evaluation_task))) {
        pthread_mutex_lock(&m.slot_mutex); {
            m.active_task = _task_index;
        } pthread_mutex_unlock(&m.slot_mutex);
    }
}
