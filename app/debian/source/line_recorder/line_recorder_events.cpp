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

void LineRecorder::set_event_callback(LREventCallback callbackProc, void* user_param) {
    m.callback_proc = callbackProc;
    m.user_param = user_param;
}

void LineRecorder::notify_event(void) {
    if (m.callback_proc != nullptr) {
        (*m.callback_proc)(&m.event_result, &m.window, m.user_param);
    }
}

gboolean LineRecorder::_event_handler(GtkWidget* _widget, GdkEvent* _event, gpointer _data) {
    (OBJ_PTR(LineRecorder, _data))->event_handler(_event);
    return ((gboolean)0);
}
void LineRecorder::event_handler(GdkEvent* event) {
    GdkEventBase* event_base = (GdkEventBase*)event;
    if (find_element((double)(event_base->x), (double)(event_base->y), &m.event_result)) {
        switch (event->type) {
            case GdkEventType::GDK_MOTION_NOTIFY: {
                handle_mouse_move_event((GdkEventMotion*)event);;
            } break;

            case GdkEventType::GDK_SCROLL: {
                handle_scroll_event((GdkEventScroll*)event);;
            } break;

            case GdkEventType::GDK_DOUBLE_BUTTON_PRESS:
            case GdkEventType::GDK_TRIPLE_BUTTON_PRESS:
            case GdkEventType::GDK_BUTTON_PRESS:
            case GdkEventType::GDK_BUTTON_RELEASE: {
                handle_mouse_button_event((GdkEventButton*)event);;
            } break;

            default: {
            } break;
        }
    }
}

bool LineRecorder::process_event_time(uint32_t timeValue) {
    if (m.event_timestamp_ms == 0) {
        m.event_timestamp_ms = (uint32_t)std::max(0, (int)timeValue - 1000);
    }
    m.event_time_s = std::min(1.0f, std::max(0.0f, (float)(timeValue - m.event_timestamp_ms) / 1000.0f));
    m.event_timestamp_ms = timeValue;
    return (false);
}

bool LineRecorder::process_mouse_clicks(void) {
    bool must_update = false;

    if ((m.fAuxKeyDown == 0) && (m.fMultiClick == 0) && (m.event_time_s > 0.5f)) {
        if (m.fLeftBtnDown == 1) {
            must_update |= select_channel();
        } else if (m.fRightBtnDown == 1) {
            must_update |= begin_mouse_zoom();
        } else if (m.fMidBtnDown == 1) {
            // still unused!
        }
    } else if ((m.fLeftBtnDown == 1) && (m.fDoubleClick == 1)) {
        if (m.event_result.m.type == LRElementType::scale) {
            m.default_scale.reset();
            init_times();
        }
        resume_autoscroll();
        must_update = true;
    }

    return (must_update);
}

void LineRecorder::process_event_state(guint _state) {
    m.fShiftKeyDown = (_state & KEY_STATE_EX_SHIFT)   ? 1 : 0;
    m.fCtrlKeyDown  = (_state & KEY_STATE_EX_CONTROL) ? 1 : 0;
    m.fAltKeyDown   = (_state & KEY_STATE_EX_ALT)     ? 1 : 0;
}

bool LineRecorder::process_key_states(GdkEventButton* _btn_event) {
    bool must_update = false;

    process_event_state(_btn_event->state);

    if (_btn_event->type == GdkEventType::GDK_BUTTON_PRESS) {
        m.fLeftBtnDown  = ((int)_btn_event->button == KEY_BUTTON_MOUSE_LEFT)   ? 1 : 0;
        m.fMidBtnDown   = ((int)_btn_event->button == KEY_BUTTON_MOUSE_MIDDLE) ? 1 : 0;
        m.fRightBtnDown = ((int)_btn_event->button == KEY_BUTTON_MOUSE_RIGHT)  ? 1 : 0;
        if (m.event_time_s > 0.5f) {
            m.fDoubleClick  = 0;
            m.fTrippleClick = 0;
        }
        m.pt_event.set(_btn_event->x, _btn_event->y);
        m.event_window.set(m.window);
    } else if (_btn_event->type == GdkEventType::GDK_BUTTON_RELEASE) {
        m.pt_current.set(_btn_event->x, _btn_event->y);
        must_update |= handle_button_release();
    } else if (_btn_event->type == GdkEventType::GDK_DOUBLE_BUTTON_PRESS) {
        m.fDoubleClick  = 1;
        m.fTrippleClick = 0;
    } else if (_btn_event->type == GdkEventType::GDK_TRIPLE_BUTTON_PRESS) {
        m.fDoubleClick  = 0;
        m.fTrippleClick = 1;
    }

    return (must_update);
}

bool LineRecorder::handle_button_release(void) {
    if (m.fZoomRectangle == 1) {
        end_mouse_zoom();
    }

    m.fLeftBtnDown = 0;
    m.fMidBtnDown = 0;
    m.fRightBtnDown = 0;
    if (m.event_time_s > 0.5f) {
        m.fDoubleClick = 0;
        m.fTrippleClick = 0;
    }

    return (true);
}

void LineRecorder::handle_mouse_button_event(GdkEventButton* _btn_event) {
    bool must_update = false;

    must_update |= process_event_time((uint32_t)(_btn_event->time));
    must_update |= process_key_states(_btn_event);
    must_update |= process_mouse_clicks();

    if (must_update) {
        update();
        notify_event();
    }
}

void LineRecorder::handle_mouse_move_event(GdkEventMotion* _motion_event) {
    float event_time_s = std::min(1.0f, std::max(0.0f, (float)(_motion_event->time - m.event_timestamp_ms) / 1000.0f));
    m.pt_current.set(_motion_event->x, _motion_event->y);

    if (m.fZoomRectangle == 1) {
        process_zoom_rect();
    }

    if ((m.fLeftBtnDown == 1) && (event_time_s >= 0.1f)) {
        suspend_autoscroll(LR_SUPEND_TO_RESTART_DEF_S);

        double timeOffset = (double)(m.pt_current.y - m.pt_event.y)
                          * m.event_window.time.get_span() / (double)m.rc.paper.height;

        double prevOffset = m.window.time.end - m.event_window.time.end;
        scroll_paper_by_dots(scroll_paper(timeOffset - prevOffset));
    }
}

void LineRecorder::handle_scroll_event(GdkEventScroll* _scroll_event) {
    float event_time_s = std::min(1.0f, std::max(0.0f, (float)(_scroll_event->time - m.event_timestamp_ms) / 1000.0f));
    m.pt_current.set(_scroll_event->x, _scroll_event->y);
    
    process_event_state(_scroll_event->state);

    if (m.fCtrlKeyDown == 1) {
        double range = m.window.time.get_span();
        if (_scroll_event->delta_y < 0) {
            range = std::max(TC_SECOND, range * 0.9);
        } else if (_scroll_event->delta_y > 0) {
            range = range * 1.1;
        }
        m.window.time.begin = m.window.time.end - range;
    } else if (m.fAltKeyDown == 1) {
        // still unused!
    } else {
        suspend_autoscroll(LR_SUPEND_TO_RESTART_DEF_S);
        double multiplier = (m.fShiftKeyDown == 1) ? 300.0 : 30.0;
        double timeOffset = _scroll_event->delta_y * TC_SECOND * multiplier;
        scroll_paper_by_dots(scroll_paper(-timeOffset));
    }
}

bool LineRecorder::begin_mouse_zoom(void) {
    if (m.fZoomRectangle == 1) {
        return (false);
    }

    if (m.event_result.m.type == LRElementType::paper) {
        m.fZoomScale = 0;
        m.fZoomPaper = 1;
    } else if (m.event_result.m.type == LRElementType::paper) {
        m.fZoomScale = 1;
        m.fZoomPaper = 0;
    } else {
        return (false);
    }

    m.fZoomRectangle = 1;
    m.rc_zoom.set(&m.pt_event);

    return (true);
}

void LineRecorder::process_zoom_rect(void) {
    if (m.fZoomRectangle == 1) {
        double x1 = std::min((double)m.pt_current.x, (double)m.pt_event.x);
        double y1 = std::min((double)m.pt_current.y, (double)m.pt_event.y);
        double x2 = std::max((double)m.pt_current.x, (double)m.pt_event.x);
        double y2 = std::max((double)m.pt_current.y, (double)m.pt_event.y);
        m.rc_zoom.set(x1, y1, x2 - x1, y2 - y1);
    }
}

bool LineRecorder::end_mouse_zoom(void) {
    if (m.pt_current.equals(m.pt_event)) {
        return (false);
    }

    process_zoom_rect();
    m.fZoomRectangle = 0;

    if (m.rc_zoom.is_too_small(LR_ZOOM_RECT_MIN_SIZE, LR_ZOOM_RECT_MIN_SIZE)) {
        return (false);
    }

    suspend_autoscroll(LR_SUPEND_TO_RESTART_DEF_S);

    double time_span   = m.window.time.get_span();
    double time_factor = time_span / (double)m.rc.paper.height;
    double time_end    = m.window.time.end - (((double)m.rc_zoom.y - (double)m.rc.paper.y) * time_factor);
    double time_range  = (double)m.rc_zoom.height * time_factor;
    double time_begin  = time_end - time_range;

    double zoom_begin  = m.default_scale.get_zoom_begin();
    double zoom_range  = m.default_scale.get_zoom_range();
    double level_begin = ((double)(m.rc_zoom.x - m.rc.paper.x) / (double)m.rc.paper.width) * zoom_range + zoom_begin;
    double level_end   = ((double)m.rc_zoom.width * zoom_range / (double)m.rc.paper.width) + level_begin;
    
    m.window.time.set(time_begin, time_end);
    m.window.level.set(level_begin, level_end);
    m.default_scale.set_zoom_window(level_begin, level_end);

    return (true);
}
