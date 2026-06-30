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

void LineRecorder::create_rectangles(int x, int y, int width, int height) {
    m.rc.surface.set(x, y, width, height);

    m.rc.scaleBox.set(&m.rc.surface);
    m.rc.scaleBox.height = LR_SCALE_BOX_HEIGHT;

    m.rc.scale.x      = m.rc.scaleBox.x + LR_SCALE_INDENT_X_LEFT;
    m.rc.scale.y      = m.rc.scaleBox.y + LR_SCALE_INDENT_Y_TOP;
    m.rc.scale.width  = m.rc.scaleBox.width - LR_SCALE_INDENT_X_LEFT - LR_SCALE_INDENT_X_RIGHT;
    m.rc.scale.height = m.rc.scaleBox.height - LR_SCALE_INDENT_Y_TOP - LR_SCALE_INDENT_Y_BOTTOM - LR_SCALE_BORDER_HEIGHT;

    m.rc.scaleBorder.set(&m.rc.scaleBox);
    m.rc.scaleBorder.y = m.rc.scale.y2(); 
    m.rc.scaleBorder.height = LR_SCALE_BORDER_HEIGHT;

    m.rc.paperBox.set(&m.rc.surface);
    m.rc.paperBox.y = m.rc.scaleBox.y2();
    m.rc.paperBox.height = height - m.rc.paperBox.y - LR_INFO_BOX_HEIGHT;

    m.rc.paperRollTop.set(&m.rc.paperBox);
    m.rc.paperRollTop.height = LR_PAPER_ROLLER_HEIGHT;

    m.rc.paper.x      = m.rc.scale.x;
    m.rc.paper.y      = m.rc.paperBox.y;
    m.rc.paper.width  = m.rc.scale.width;
    m.rc.paper.height = m.rc.paperBox.height;

    m.rc.paperRollBottom.set(&m.rc.paperBox);
    m.rc.paperRollBottom.y = m.rc.paperBox.y2() - LR_PAPER_ROLLER_HEIGHT;
    m.rc.paperRollBottom.height = LR_PAPER_ROLLER_HEIGHT;

    m.rc.infoBox.set(&m.rc.surface);
    m.rc.infoBox.y = y + height - LR_INFO_BOX_HEIGHT;
    m.rc.infoBox.height = LR_INFO_BOX_HEIGHT;

    m.rc.infoBorder.set(&m.rc.infoBox);
    m.rc.infoBorder.height = LR_SCALE_BORDER_HEIGHT;

    m.rc.info.set(&m.rc.infoBox);
    m.rc.info.x      += LR_SCALE_INDENT_X_LEFT;
    m.rc.info.y      += LR_SCALE_INDENT_Y_TOP;
    m.rc.info.width  -= (LR_SCALE_INDENT_X_LEFT + LR_SCALE_INDENT_X_RIGHT);
    m.rc.info.height -= (LR_SCALE_INDENT_Y_TOP + LR_SCALE_INDENT_Y_BOTTOM);
}

void LineRecorder::init_colors(void) {
    m.color.scaleBkg    = RGBA(247, 247, 247, 255);
    m.color.scaleText   = RGBA(  0,   0,   0, 255);
    m.color.paper       = RGBA(255, 255, 255, 255);
    m.color.paperText   = RGBA( 63,   0, 127, 255);
    m.color.gridFine    = RGBA(223, 223, 223, 255);
    m.color.gridMain    = RGBA(191, 191, 191, 255);
    m.color.infoTextDef = RGBA(  0,   0,   0, 255);
    m.color.infoTextHi  = RGBA( 63,   0, 127, 255);
}

void LineRecorder::draw_paper(cairo_t* cr) {
    cairo_save(cr);
    m.rc.paperBox.clip(cr);

    Times times(m.window.time.end);

    cairo_select_font_face(cr, DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_text_extents_t extents{ 0 };
    cairo_text_extents(cr, times.to_date_and_time_hms(" / ").c_str(), &extents);

    if (m.segment != nullptr) {
        RectEx rc(&m.rc.paperBox);
        rc.y = m.rc.paper.y;
        rc.height = m.rc.paper.height;
        rc.clip(cr);
        for (int y = m.rc.paper.y1() - LR_PAPER_SEGMENT_HEIGHT; y <= m.rc.paper.y2() + LR_PAPER_SEGMENT_HEIGHT; y += LR_PAPER_SEGMENT_HEIGHT) {
            int y1 = y + m.paper_shift;

            cairo_set_source_surface(cr, m.segment, m.rc.paperBox.x, y1);
            cairo_paint(cr);

            long double pixelOffset = (double)y1 - m.rc.paper.y;
            long double timeOffset = pixelOffset * (long double)m.window.time.get_span() / (long double)m.rc.paper.height;
            double timeCodeFor_y1 = (double)((long double)m.window.time.end - timeOffset);
            times.set_timecode(timeCodeFor_y1);
            _cairo_set_source_rgb_a(cr, m.color.paperText, 0.33);
            cairo_move_to(cr, m.rc.info.x2() - extents.width - 16, y1); // + extents.height);
            cairo_show_text(cr, times.to_date_and_time_hms(" / ").c_str());
        }
    }

    cairo_restore(cr);
}

void LineRecorder::draw_info(cairo_t* cr) {
    cairo_save(cr);
    m.rc.infoBox.clip(cr);

    _cairo_set_source_rgb(cr, m.color.scaleBkg);
    cairo_paint(cr);

    m.rc.infoBorder.set(cr);
    cairo_set_source(cr, m.pattern.infoBorder);
    cairo_fill(cr);

    _cairo_set_source_rgb(cr, m.color.scaleText);
    cairo_select_font_face(cr, DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    int fontSize = (int)(m.rc.info.height * 0.25);
    cairo_set_font_size(cr, fontSize);
    int x = m.rc.info.x;
    int y = m.rc.info.y;
    int max_x = 0;

    // @todo: Draw the info field for the selected channel.
    
    Times wndBegin(m.window.time.begin);
    Times wndEnd(m.window.time.end);
    std::string strTimeBegin;
    std::string strTimeEnd;
    double range = m.window.time.get_span();
    if (range >= 1.0) {
        strTimeBegin = wndBegin.to_date_and_time_hms(" / ");
        strTimeEnd = wndEnd.to_date_and_time_hms(" / ");
    } else {
        strTimeBegin = wndBegin.to_time_hms();
        strTimeEnd = wndEnd.to_time_hms();
    }
    std::string strTimeSpan = Times::format_timespan(m.window.time.get_span());
    draw_info_section(cr, x, y, max_x, "Paper window:", strTimeEnd.c_str(), strTimeBegin.c_str(), strTimeSpan.c_str());
    m.rc.infoWnd.set(x, m.rc.info.y + 4, max_x - x + 4, m.rc.info.height - 8);
    m.rc.infoWnd.fill(cr, RGBA(0, 0, 255, 15), 0.1f);

    x = max_x + 8;
    y = m.rc.info.y;
    m.rc.infoSel.set(x, y, m.rc.info.x + m.rc.info.width - x, m.rc.info.height);

    if (m.event_result.m.type != LRElementType::invalid) {
        draw_selection_info(cr, m.rc.infoSel, m.event_result);
    }

    cairo_restore(cr);
}

void LineRecorder::draw_control_helpers(cairo_t* cr) {
    cairo_save(cr);

    if (m.event_result.m.subtype == LRElementSub::curve_point) {
        _cairo_set_source_rgb_a(cr, C_BLUE, 0.25f);
        cairo_arc(cr, m.event_result.m.found_pt.x, m.event_result.m.found_pt.y, m.event_result.m.found_sub.width, 0.0, 360.0);
        cairo_fill(cr);
    } else if ( (m.event_result.m.subtype == LRElementSub::info_file) ||
                (m.event_result.m.subtype == LRElementSub::info_wnd)  ||
                (m.event_result.m.subtype == LRElementSub::info_sel)) {
        m.event_result.m.found_sub.fill(cr, C_BLUE, 0.25f);
    }

    if (m.fZoomRectangle == 1) {
        if (m.fZoomPaper == 1) {
            m.rc.paperBox.clip(cr);
            m.rc_zoom.draw_frame(cr, m.color.paperText, 0.5);
        } else if (m.fZoomScale == 1) {
            m.rc.scaleBox.clip(cr);
            m.rc_zoom.draw_frame(cr, m.color.paperText, 0.5);
        }
    }

    cairo_restore(cr);
}

void LineRecorder::update_segment(void) {
    if (m.segment == nullptr) return;

    RectEx rc(m.rc.segment);

    cairo_t* cr = cairo_create(m.segment);

    _cairo_set_source_rgb(cr, m.color.paper);
    cairo_paint(cr);

    // Paper feed holes
    int xc[2];
    xc[0] = rc.x + ((m.rc.paper.x - m.rc.paperBox.x) / 2);
    xc[1] = rc.x + rc.width - ((m.rc.paper.x - m.rc.paperBox.x) / 2);
    int yc = rc.y + (rc.height / 2);
    int r = xc[0] / 2;

    for (int i = 0; i < 2; i++) {
        _cairo_set_source_rgb(cr, RGBA(79, 79, 79, 255));
        cairo_arc(cr, xc[i], yc, r, 0.0, 360.0);
        cairo_fill(cr);
        cairo_set_line_width(cr, 1.0);
        cairo_arc(cr, xc[i], yc, r, 0.0, 360.0);
        _cairo_set_source_rgb(cr, RGBA(191, 191, 191, 255));
        cairo_stroke(cr);
    }

    // Paper line, horizontal
    cairo_set_line_width(cr, 1.0);
    _cairo_set_source_rgb(cr, m.color.gridMain);
    int count = 4;
    for (int i = 0; i < count; i++) {
        int y = rc.y + (int)((double)i * (double)LR_PAPER_SEGMENT_HEIGHT / (double)count);
        cairo_move_to(cr, m.rc.paper.x, y);
        cairo_line_to(cr, m.rc.paper.x + m.rc.paper.width, y);
        cairo_stroke(cr);
        _cairo_set_source_rgb(cr, m.color.gridFine);
    }

    // Scale dividers
    cairo_set_line_width(cr, 0.5);
    for (ScaleStep& step : m.select.scale_steps) {
        if (step.f_main_divider == 1) {
            cairo_stroke(cr);
            _cairo_set_source_rgb(cr, m.color.gridMain);
        } else {
            cairo_stroke(cr);
            _cairo_set_source_rgb(cr, m.color.gridFine);
        }
        cairo_move_to(cr, step.pos, rc.y);
        cairo_line_to(cr, step.pos, rc.y + rc.height);
    }
    cairo_stroke(cr);

    cairo_destroy(cr);
}

void LineRecorder::draw_channels(cairo_t* cr) {
    cairo_save(cr);
    m.rc.paper.clip(cr);

    for (Evaluator*& evaluator : m.evaluations) {
        std::vector<EvalCurve*> curves;
        if (evaluator->create_curves(curves, cr, m.window, true)) {
            evaluator->draw_curves(cr, m.rc.paper, m.color.paper, curves);
        }
    }

    cairo_restore(cr);

    m.rc.paperRollTop.set(cr);
    cairo_set_source(cr, m.pattern.paperTopRoll);
    cairo_fill(cr);  

    m.rc.paperRollBottom.set(cr);
    cairo_set_source(cr, m.pattern.paperBottomRoll);
    cairo_fill(cr);  
}

void LineRecorder::draw_info_section(cairo_t* cr, int x, int y, int& max_x, const char* hint, const char* part1, const char* part2, const char* part3) {
    cairo_text_extents_t extents{0};

    cairo_text_extents(cr, "TEST", &extents);
    int textHeight = extents.height + 4;
    
    if (hint != nullptr) {
        _cairo_set_source_rgb_a(cr, m.color.infoTextDef, 1.0);
        cairo_text_extents(cr, hint, &extents);
        max_x = std::max(max_x, x + (int)extents.width);
        y += textHeight;
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, hint);
    }

    int y_a = y + 4;
    int x_t = x + 4;

    _cairo_set_source_rgb_a(cr, m.color.infoTextHi, 0.75);

    if (part1 != nullptr) {
        cairo_text_extents(cr, part1, &extents);
        x_t = std::max(x_t, x + (int)extents.width);
        max_x = std::max(max_x, x_t);
        y += textHeight;
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, part1);
    }

    if (part2 != nullptr) {
        cairo_text_extents(cr, part2, &extents);
        x_t = std::max(x_t, x + (int)extents.width);
        max_x = std::max(max_x, x_t);
        y += textHeight;
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, part2);
    }
    
    if (part3 != nullptr) {
        _cairo_set_source_rgb_a(cr, m.color.infoTextDef, 1.0);
        cairo_set_line_width(cr, 1.0);

        x_t += 4;
        int y_b = y + 2;
        cairo_move_to(cr, x_t, y_a);
        cairo_line_to(cr, x_t + 3, y_a);
        cairo_line_to(cr, x_t + 3, y_b);
        cairo_line_to(cr, x_t, y_b);
        cairo_stroke(cr);
    
        y = (y_a + y_b) / 2;
        cairo_move_to(cr, x_t + 3, y);
        cairo_line_to(cr, x_t + 6, y);
        cairo_stroke(cr);
        max_x = std::max(max_x, x_t + 8);

        cairo_text_extents(cr, part3, &extents);
        cairo_move_to(cr, x_t + 8, y + (extents.height / 2));
        cairo_show_text(cr, part3);

        max_x += extents.width;
    }

    max_x += 8;
}

void LineRecorder::draw_selection_info(cairo_t *cr, RectEx& rc, LRFindResult& result) {
    cairo_save(cr);
    rc.clip(cr);

    int x = rc.x;
    int y = rc.y;
    int x_max = 0;

    draw_info_section(cr, x, y, x_max, "Selection:", result.get_TypeName(), result.get_SubTypeName(), nullptr);
    y = rc.y;
    x = x_max + 8;

    // @todo: Draw the info for the selection rectangle according to the selected channel.

    rc.y += 4;
    rc.height -= 8;
    rc.fill(cr, RGBA(0, 255, 0, 255), 0.1f);

    cairo_restore(cr);
}

void LineRecorder::draw_scale(cairo_t* cr) {
    ScaleDrawing* scale = (ScaleDrawing*)&m.select.scale;

    EvalCurve* curve = get_curve(m.select.key.c_str());
    if (curve != nullptr) {
        scale->set_value(curve->get_value_at_timecode(m.window.time.end));
    }

    ColorRef color_pointer = ScaleDrawing::check_color_on_background(scale->get_color_ref(), m.color.scaleBkg);

    scale->draw(ScaleLayout::normal_horizontal, ScalePointerType::pointer,
        cr, &m.rc.scaleBox, &m.rc.scale, m.color.scaleBkg, m.color.scaleText,
        color_pointer, m.select.key.c_str(), m.select.scale_steps);

    update_segment();

    m.rc.scaleBorder.set(cr);
    cairo_set_source(cr, m.pattern.scaleBorder);
    cairo_fill(cr);
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
