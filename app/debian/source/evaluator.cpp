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
    m.ext_window.expand(0.5);

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

// std::string twnd_begin = Times::format(InFormat::dateAndTime, m.ext_window.time.begin);
// std::string twnd_end   = Times::format(InFormat::dateAndTime, m.ext_window.time.end);
// printf("\nEvaluation window: %s to %s\n", twnd_begin.c_str(), twnd_end.c_str());
// print_slot(m.active_slot, m.evaluation_slot[m.active_slot]); // ****
}

size_t Evaluator::create_curve_list(std::vector<PointF*>& _curve_list, RectEx& _rect, LogWindow _window, bool _vertical) {
    pthread_mutex_lock(&m.slot_mutex); {

    // if (m.current_slot != nullptr) {
    //     for (int channel_index = 0; channel_index < LOG_SLOT_MAX;  channel_index++) {
    //         EvalPt* channel = m.current_slot->get_channel(channel_index, false);
    //         if (channel != nullptr) {
    //             LogWindow slot_window(m.current_slot->get_window());

    //             int count = 0;
    //             for (int i = 0; i < LOG_EVAL_CURVE_LEN_MAX; i++) {
    //                 if (channel[i].is_used()) {
    //                     count++;
    //                 }
    //             }

    //             size_t size = sizeof (PointF) * (size_t)count;
    //             PointF* curve = (PointF*)malloc(size);

    //             double t_begin = _window.time.get_begin();
    //             double t_span  = _window.time.get_span();
    //             double v_begin = _window.level.get_begin();
    //             double v_span  = _window.level.get_span();

    //             int n = 0;
    //             for (int i = 0; (i < LOG_EVAL_CURVE_LEN_MAX) && (n < count); i++) {
    //                 if (channel[i].is_used()) {
    //                     if (_vertical) {
    //                         curve[n++].set( ((channel[i].get_timecode() - t_begin) * (double)_rect.height / t_span) + (double)_rect.y,
    //                                         ((channel[i].get_value()    - v_begin) * (double)_rect.width  / v_span) + (double)_rect.x );
    //                     } else {
    //                         curve[n++].set( ((channel[i].get_timecode() - t_begin) * (double)_rect.width  / t_span) + (double)_rect.x,
    //                                         ((channel[i].get_value()    - v_begin) * (double)_rect.height / v_span) + (double)_rect.y );
    //                     }
    //                 }
    //             }

    //             _curve_list.push_back(curve);
    //         }
    //     }
    // }

    } pthread_mutex_unlock(&m.slot_mutex);
    
    return (_curve_list.size());
}

void Evaluator::delete_curve_list(std::vector<PointF*>& _curve_list) {
    for (PointF*& curve : _curve_list) {
        if (curve != nullptr) {
            free(curve);
            curve = nullptr;
        }
    }
    _curve_list.clear();
}

void Evaluator::print_slot(int n, EvaluationSlot* _slot) {
    printf("Slot %d:\n", n);
    if (_slot != nullptr) {
        for (int chn = 0; chn < LOG_SLOT_MAX; chn++) {
            EvalPt* channel = _slot->get_channel(chn, false);
            if (channel != nullptr) {
                printf("chn %d: ", chn);
                for (int i = 0; i < LOG_EVAL_CURVE_LEN_MAX; i++) {
                    if (channel[i].is_used()) {
                        printf("%-3d, ", (int)channel[i].get_value());
                    } else {
                        printf("---, ");
                    }
                }
                printf("\n");
            }
        }
    }
}
