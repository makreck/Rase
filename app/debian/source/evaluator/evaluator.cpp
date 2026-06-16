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
    m.fd = -1;
    m.logfile = new LogFile();
    
    pthread_mutex_init(&m.slot_mutex, nullptr);

    if (Files::open_file(m.fd, m.path.c_str(), O_RDWR)) {
        m.logfile->get(m.fd);
    }
}

void Evaluator::cleanup(void) {
    pthread_mutex_lock(&m.slot_mutex); {
        delete_curve_list();

        for (int i = 0; i < SIZEOFARRAY(m.evaluation_slot); i++) {
            if (m.evaluation_slot[i] != nullptr) {
                delete (m.evaluation_slot[i]);
                m.evaluation_slot[i] = nullptr;
            }
        }

        if (m.logfile != nullptr) {
            if (m.fd != -1) {
                Files::close_file(m.fd);
            }
            delete (m.logfile);
        }
        
    } pthread_mutex_unlock(&m.slot_mutex);

    pthread_mutex_destroy(&m.slot_mutex);
}

const char* Evaluator::get_path(void) {
    return (m.path.c_str());
}

void Evaluator::set_window(LogWindow _window) {
    m.window = _window;
    m.ext_window = m.window;
    m.ext_window.expand(LOG_DISPLAY_WINDOW_EXPAND_FACTOR);

    pthread_mutex_lock(&m.slot_mutex); {
        m.logfile->get(m.fd);

        if (m.evaluation_slot[m.active_slot] != nullptr) {
            delete (m.evaluation_slot[m.active_slot]);
            m.evaluation_slot[m.active_slot] = nullptr;
        }

        m.evaluation_slot[m.active_slot] = new EvaluationSlot(m.fd, m.logfile, &m.ext_window);

        m.active_slot++;
        if (m.active_slot >= SIZEOFARRAY(m.evaluation_slot)) {
            m.active_slot = 0;
        }
    } pthread_mutex_unlock(&m.slot_mutex);
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

void Evaluator::draw_curves(cairo_t* _cr, RectEx& _rect, LogWindow _window, bool _vertical) {
    pthread_mutex_lock(&m.slot_mutex); {

        delete_curve_list();

        if (m.evaluation_slot[m.active_slot] != nullptr) {
            for (int channel_index = 0; channel_index < LOG_SLOT_MAX;  channel_index++) {

                Scale* scale = m.logfile->get_inventory()->get_slot(channel_index);
                if (scale == nullptr) continue;

                EvalPt* channel = m.evaluation_slot[m.active_slot]->get_channel(channel_index, false);
                if (channel != nullptr) {
                    LogWindow slot_window(m.evaluation_slot[m.active_slot]->get_window());

                    int count = 0;
                    for (int i = 0; i < LOG_EVAL_CURVE_LEN_MAX; i++) {
                        if (channel[i].is_used()) {
                            count++;
                        }
                    }

                    if (count > 0) {
                        EvalCurve* curve = new EvalCurve(count, channel_index, scale->get_color_ref(), scale->get_line_width());
                        if (curve == nullptr) { continue; }

                        double t_begin = _window.time.get_begin();
                        double t_end   = _window.time.get_end();
                        double t_span  = _window.time.get_span();
                        double v_begin = 0.0; // _window.level.get_begin();
                        double v_span  = 1.0; // _window.level.get_span();

                        int n = 0;
                        for (int i = 0; (i < LOG_EVAL_CURVE_LEN_MAX) && (n < count); i++) {
                            float norm_value = scale->get_zoom_normalized(channel[i].get_value());

                            if (channel[i].is_used()) {
                                double timecode = channel[i].get_timecode();
                                if (_vertical) {
                                    curve->set(n++, ((norm_value - v_begin) * (double)_rect.width  / v_span) + (double)_rect.x,
                                                    ((t_end - timecode) * (double)_rect.height / t_span) + (double)_rect.y);
                                } else {
                                    curve->set(n++, ((timecode - t_begin) * (double)_rect.width  / t_span) + (double)_rect.x,
                                                    ((norm_value - v_begin) * (double)_rect.height / v_span) + (double)_rect.y);
                                }
                            }
                        }

                        m.curve_list.push_back(curve);
                    }
                }
            }
        }

        for (EvalCurve*& curve : m.curve_list) {
            curve->draw(_cr);
        }

    } pthread_mutex_unlock(&m.slot_mutex);
}
