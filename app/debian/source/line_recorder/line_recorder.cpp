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

void LineRecorder::init(LREventCallback callback_proc, void* user_param) {
    set_event_callback(callback_proc, user_param);
    init_colors();
    init_times();

    m.default_scale.set_defaults();
    init_drawing_area();

    pthread_create(&m.update_thread_handle, nullptr, LineRecorder::_update_thread, this);

    gtk_widget_queue_draw(m.area);
}

void LineRecorder::cleanup(void) {
    if (m.update_thread_handle != INVALID_THREAD_HANDLE) {
        pthread_cancel(m.update_thread_handle);
        pthread_join(m.update_thread_handle, nullptr);
        m.update_thread_handle = INVALID_THREAD_HANDLE;
    }

    if (m.evaluations.size() > 0) {
        for (Evaluator*& evaluator : m.evaluations) {
            if (evaluator != nullptr) {
                delete (evaluator);
                evaluator = nullptr;
            }
        }
        m.evaluations.clear();
    }

    m.area = nullptr;

    if (m.surface != nullptr) {
        cairo_surface_destroy(m.surface);
        m.surface = nullptr;
    }

    destroy_segment();
    destroy_gradient_patterns();
}

void LineRecorder::update(void) {
    if (GTK_IS_WIDGET(m.area)) {
        redraw();
        gtk_widget_queue_draw(m.area);
    }
}

void LineRecorder::redraw(void) {
    if (m.surface == nullptr) return;

    cairo_t* cr = cairo_create(m.surface);
    draw_scale(cr);
    draw_paper(cr);
    draw_info(cr);
    draw_channels(cr);
    draw_control_helpers(cr);
    cairo_destroy(cr);
}

void LineRecorder::init_times(void) {
    Times t(TimeInitializer::now);
    double t2 = t.get_timecode();
    double t1 = t2 - LR_INIT_TIMESPAN;
    m.window.set(t1, t2, 0.0, 1.0);
    m.fAutoScroll = 1;
}

void LineRecorder::init_drawing_area(void) {
    m.area = gtk_drawing_area_new();
    gtk_widget_set_size_request(m.area, 128, 128);
    g_signal_connect(m.area, "configure-event", G_CALLBACK(LineRecorder::_configure), this);
    g_signal_connect(m.area, "draw", G_CALLBACK(LineRecorder::_draw_function), this);
    GtkTool::set_mouse_button_event_list(m.area, G_CALLBACK(LineRecorder::_event_handler), this);
}

void LineRecorder::suspend_autoscroll(float seconds) {
    m.suspend_timestamp = Times::get_tick_count64() + (uint64_t)(fabs(seconds) * 1000.0);
    m.fAutoScroll = 0;
}

void LineRecorder::resume_autoscroll(void) {
    m.suspend_timestamp = 0;
    m.fAutoScroll = 1;
}

void LineRecorder::scroll_paper_by_dots(double _dotsToScroll) {
    double newShift = m.paper_shift + _dotsToScroll;
    while (newShift < 0.0) {
        newShift += (double)LR_PAPER_SEGMENT_HEIGHT;
    }
    while (newShift > ((double)LR_PAPER_SEGMENT_HEIGHT)) {
        newShift -= (double)LR_PAPER_SEGMENT_HEIGHT;
    }
    m.paper_shift = newShift;
}

double LineRecorder::paper2now(void) {
    double tc_moved       = m.window.time.approximate(top_of_paper(), m.update_speed_s * TC_SECOND);
    double tc_timeSpan    = m.window.time.get_span();
    double paperLength    = (double)m.rc.paper.height;
    double pixelsToScroll = tc_moved * paperLength / tc_timeSpan;
    return (pixelsToScroll);
}

double LineRecorder::scroll_paper(double timeOffset) {
    double tc_moved       = m.window.time.approximate(m.window.time.end + timeOffset, m.update_speed_s * TC_SECOND);
    double tc_timeSpan    = m.window.time.get_span();
    double paperLength    = (double)m.rc.paper.height;
    double pixelsToScroll = tc_moved * paperLength / tc_timeSpan;
    return (pixelsToScroll);
}

gboolean LineRecorder::_configure(GtkWidget* _widget, cairo_t* _cr, gpointer _data) {
    (reinterpret_cast<LineRecorder*>(_data))->configure(_widget, _cr);
    return ((gboolean)0);
}
void LineRecorder::configure(GtkWidget* widget, cairo_t* cr) {
    if (m.surface != nullptr) {
        cairo_surface_destroy(m.surface);
    }

    destroy_gradient_patterns();
    destroy_segment();

    create_rectangles(0, 0, gtk_widget_get_allocated_width(widget), gtk_widget_get_allocated_height(widget));
    
    m.surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, m.rc.surface.width, m.rc.surface.height);

    create_segment();
    create_gradient_patterns();

    redraw();
}

void LineRecorder::create_gradient_patterns(void) {
    ColorRef scaleBkg = RGB_ALPHA(m.color.scaleBkg, 127);
    ColorRef paper = RGB_ALPHA(m.color.paper, 127);
    ColorRef roll = RGBA(0, 0, 0, 223);

    m.pattern.scaleBorder     = m.rc.scaleBorder.linear_gradient(RectDir::vertical, scaleBkg, roll);
    m.pattern.paperTopRoll    = m.rc.paperRollTop.linear_gradient(RectDir::vertical, roll, paper);
    m.pattern.paperBottomRoll = m.rc.paperRollBottom.linear_gradient(RectDir::vertical, paper, roll);
    m.pattern.infoBorder      = m.rc.infoBorder.linear_gradient(RectDir::vertical, roll, scaleBkg);
}

void LineRecorder::destroy_gradient_patterns(void) {
    for (cairo_pattern_t*& pattern : m.patternList) {
        if (pattern != nullptr) {
            cairo_pattern_destroy(pattern);
            pattern = nullptr;
        }
    }
}

void LineRecorder::create_segment(void) {
    m.rc.segment.set(m.rc.paperBox);
    m.rc.segment.y = 0;
    m.rc.segment.height = LR_PAPER_SEGMENT_HEIGHT;
    m.segment = cairo_image_surface_create(cairo_format_t::CAIRO_FORMAT_ARGB32, m.rc.segment.width, m.rc.segment.height);
}

void LineRecorder::destroy_segment(void) {
    if (m.segment == nullptr) return;
    cairo_surface_destroy(m.segment);
    m.segment = nullptr;
}

gboolean LineRecorder::_draw_function(GtkWidget* _area, cairo_t* _cr, gpointer _data) {
    (reinterpret_cast<LineRecorder *>(_data))->draw_function(_area, _cr);
    return ((gboolean)0);
}
void LineRecorder::draw_function(GtkWidget* area, cairo_t* cr) {
    if (m.surface != nullptr) {
        cairo_set_source_surface(cr, m.surface, 0, 0);
        cairo_paint(cr);
    }
}

void LineRecorder::set_found_on_scale(double _x, double _y, LRFindResult& result) {
    result.m.type    = LRElementType::scale;
    result.m.subtype = LRElementSub::standard;

    result.m.found_rect.set(m.rc.scale);
    result.m.found_pt.set(_x - (double)m.rc.scale.x, _y - (double)m.rc.scale.y);
    result.m.timecode = Times::get_now();

    // @todo: Enable scale for found channel and set the scale pointer to the head of recording display.
}

void LineRecorder::set_found_on_info(double _x, double _y, LRFindResult& result) {
    result.m.type    = LRElementType::info;
    result.m.subtype = LRElementSub::standard;

    result.m.found_rect.set(m.rc.info);
    result.m.found_pt.set(_x - m.rc.info.x, _y - m.rc.info.y);
    result.m.timecode = Times::get_now();

    if (m.rc.infoFile.is_pt_in_rect(_x, _y)) {
        result.m.subtype = LRElementSub::info_file;
        result.m.found_sub.set(&m.rc.infoFile);
    } else if (m.rc.infoWnd.is_pt_in_rect(_x, _y)) {
        result.m.subtype = LRElementSub::info_wnd;
        result.m.found_sub.set(&m.rc.infoWnd);
    } else if (m.rc.infoSel.is_pt_in_rect(_x, _y)) {
        result.m.subtype = LRElementSub::info_sel;
        result.m.found_sub.set(&m.rc.infoSel);
    }
}

void* LineRecorder::_update_thread(void* _object) {
    (reinterpret_cast<LineRecorder*>(_object))->update_thread();
    return (nullptr);
}
void LineRecorder::update_thread(void) {
    while (true) {
        Times::delay_ms((uint64_t)(m.update_speed_s * 1000.0));
        if (m.update_pending == false) {
            m.update_pending = true;
            gdk_threads_add_idle(LineRecorder::_update_callback, this);
        }
    }
}

gboolean LineRecorder::_update_callback(gpointer _object) {
    (reinterpret_cast<LineRecorder*>(_object))->update_callback();
    return ((gboolean)0);
}
void LineRecorder::update_callback(void) {
    window_update();

    if (m.fAutoScroll) {
        scroll_paper_by_dots(paper2now());
    } else {
        if (m.suspend_timestamp != 0) {
            if (Times::get_tick_count64() > m.suspend_timestamp) {
                resume_autoscroll();
            }
        }
    }

    update();
    m.update_pending = false;
}

void LineRecorder::window_update(void) {
    for (Evaluator*& evaluator : m.evaluations) {
        if (evaluator != nullptr) {
            evaluator->set_window(m.window);
        }
    }
}

double LineRecorder::top_of_paper(void) {
    return (Times::get_now() - (m.window.time.get_span() * 0.05));
}

bool LineRecorder::add_evaluation(const char* _path) {
    for (Evaluator*& evaluator : m.evaluations) {
        if (evaluator != nullptr) {
            if (strcmp(_path, evaluator->get_path()) == 0) {
                return (true);
            }
        }
    }
    m.evaluations.push_back(new Evaluator(_path));
    return (true);
}

bool LineRecorder::find_element(double _x, double _y, LRFindResult* _result) {
    bool found = false;
    LRFindResult result(_x, _y);

    if (m.rc.surface.is_pt_in_rect(_x, _y)) {
        if (m.rc.scale.is_pt_in_rect(_x, _y)) {
            set_found_on_scale(_x, _y, result);
            found = true;
        } else if (m.rc.paper.is_pt_in_rect(_x, _y)) {
            set_found_on_paper(_x, _y, result);
            found = true;
        } else if (m.rc.infoBox.is_pt_in_rect(_x, _y)) {
            set_found_on_info(_x, _y, result);
            found = true;
        }
    }

    if ((found == true) && (_result != nullptr)) {
        _result->set(&result);
    }    

    return (found);
}

void LineRecorder::set_found_on_paper(double _x, double _y, LRFindResult& result) {
    result.set_paper(_x, _y, &m.rc.paper);
    double smallest_delta = -1.0;
    
    for (Evaluator *&evaluator : m.evaluations) {
        std::vector<EvalCurve*> curves = evaluator->get_displayed_curves();
        for (EvalCurve*& curve : curves) {
            if (curve != nullptr) {
                for (size_t i = 0; i < curve->get_length(); i++) {
                    PointF* ptr = curve->get_point(i);
                    if (ptr != nullptr) {
                        PointF pt(ptr->x * m.rc.paper.width + m.rc.paper.x, ptr->y * m.rc.paper.height + m.rc.paper.y);
                        double dx = _x - pt.x;
                        double dy = _y - pt.y;
                        double delta_px = sqrt((dx * dx) + (dy * dy));
                        if ((delta_px <= LR_CAPTURE_THRESHOLD_PX) && ((smallest_delta < 0.0) || (delta_px < smallest_delta))) {
                            smallest_delta = delta_px;
                            m.headline = evaluator->get_device_serial_number();
                            result.set_curve_point(curve, i, &pt, delta_px);
                        }
                    }
                }
            }
        }
    }
}

bool LineRecorder::select_channel(void) {
    if (m.event_result.m.subtype == LRElementSub::curve_point) {
        m.default_scale.set(m.event_result.get_scale());
    }
    return (true);
}

Evaluator* LineRecorder::get_evaluator_of_device(const char* _device_serial_number) {
    for (Evaluator *&evaluator : m.evaluations) {
        if (evaluator != nullptr)  {
            const char* device_serial_number = evaluator->get_device_serial_number();
            if (strncmp(_device_serial_number, device_serial_number, 20) == 0) {
                return (evaluator);
            }
        }
    }
    return (nullptr);
}

const Scale* LineRecorder::get_selected_scale(void) {
    return (&m.default_scale);
}

float LineRecorder::get_sel_curve_value_at_top_of_window(void) {
    Evaluator* evaluator = get_evaluator_of_device(m.event_result.get_device_serial_number());
    if (evaluator != nullptr) {
        for (EvalCurve*& curve : evaluator->get_displayed_curves()) {
            if (curve != nullptr) {
                if (strcmp(m.default_scale.key, curve->scale.key) == 0) {
                    float scale_pointer = curve->get_value_at_timecode(m.window.time.end);
                    return (scale_pointer);
                }
            }
        }
    }
    return (m.default_scale.get_value());
}
