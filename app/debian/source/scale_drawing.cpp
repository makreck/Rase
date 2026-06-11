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

void ScaleDrawing::draw(ScaleLayout layout, ScalePointerType type,
    cairo_t* cr, RectEx *rc_area, RectEx *rc_scale, ColorRef color_background, ColorRef color_foreground,
    ColorRef color_pointer, const char *headline, std::vector<ScaleStep> &dividers) {

    dividers.clear();

    if ((cr == nullptr) || (rc_area == nullptr)) {
        return;
    }

    if (rc_scale == nullptr) {
        rc_scale = rc_area;
    }

    switch (layout) {
        case ScaleLayout::normal_radial: {
        } break;

        case ScaleLayout::normal_vertical: {
        } break;

        case ScaleLayout::normal_horizontal:
        default: {
            draw_scale_horizontal(type, cr, rc_area, rc_scale, color_background, color_foreground, color_pointer, headline, dividers);
        } break;
    }
}

void ScaleDrawing::draw_line_pointer_horizontal(cairo_t* cr, PointF &pointer, RectEx &rc, ColorRef color_pointer) {
    int x = rc.x + (int)(((double)rc.width * std::max(0.0, std::min(1.0, (double)pointer.pos))) + 0.5);
    cairo_set_line_width(cr, 3.0);
    cairo_set_source_rgba(cr, CR_R(color_pointer), CR_G(color_pointer), CR_B(color_pointer), 0.9);
    cairo_move_to(cr, x, rc.y);
    cairo_line_to(cr, x, rc.y2());
    cairo_stroke(cr);
}

void ScaleDrawing::draw_bar_pointer_horizontal(cairo_t* cr, PointF &pointer, RectEx &rc, ColorRef color_pointer) {
    RectEx rcFill = rc;
    rcFill.width = (int)(((double)rc.width * std::max(0.0, std::min(1.0, (double)pointer.pos))) + 0.5);
    rcFill.y = rcFill.y2() - (int)((double)rcFill.height * 0.16);
    rcFill.fill(cr, color_pointer, 0.5);
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgba(cr, CR_R(color_pointer), CR_G(color_pointer), CR_B(color_pointer), 0.9);
    cairo_move_to(cr, rcFill.x2(), rcFill.y2());
    cairo_line_to(cr, rcFill.x2(), rcFill.y - 24);
    cairo_stroke(cr);
}

void ScaleDrawing::draw_fill_pointer_horizontal(cairo_t* cr, PointF &pointer, RectEx &rc, ColorRef color_pointer) {
    RectEx rcFill = rc;
    rcFill.width = (int)(((double)rc.width * std::max(0.0, std::min(1.0, (double)pointer.pos))) + 0.5);
    rcFill.fill(cr, color_pointer, 0.5f);
}

void ScaleDrawing::draw_regular_pointer_horizontal(cairo_t* cr, PointF &pointer, RectEx &rc, ColorRef color_pointer) {
    RectEx rcFill = rc;
    rcFill.width = (int)(((double)rc.width * std::max(0.0, std::min(1.0, (double)pointer.pos))) + 0.5);
    rcFill.y = rcFill.y2() - (int)((double)rcFill.height * 0.3);
    rcFill.height += 4;

    ColorRef c1 = RGBA_WITH_ALPHA(color_pointer, 0.5f);

    int w = 5;

    _cairo_set_source_rgba(cr, c1);
    cairo_move_to(cr, rcFill.x2() - w, rcFill.y + w);
    cairo_line_to(cr, rcFill.x2() + 0, rcFill.y);
    cairo_line_to(cr, rcFill.x2() + w, rcFill.y + w);
    cairo_line_to(cr, rcFill.x2() + w, rc.y2());
    cairo_line_to(cr, rcFill.x2() - w, rc.y2());
    cairo_close_path(cr);
    cairo_fill(cr);

    _cairo_set_source_rgba(cr, C_BLACK);
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, rcFill.x2(), rcFill.y2());
    cairo_line_to(cr, rcFill.x2(), rc.center_y());
    cairo_stroke(cr);
}

void ScaleDrawing::calculate_for(ScaleLayout layout, RectEx &rc, int maxTextWidth_px, std::vector<ScaleStep> &dividers) {
    dividers.clear();

    if (maxTextWidth_px < 3) {
        maxTextWidth_px = 24;
    }

    double scaleRange = (double)get_zoom_range();
    double digits = (double)((int)((scaleRange / (double)get_step()) + 0.5));

    double base;
    double stepping;
    double power = log(digits) / log(2.0);
    if ((scaleRange >= 2.0) && ((double)((int64_t)power) == power)) {
        stepping = (double)((int64_t)power);
        base = 2.0;
    } else {
        stepping = (double)(scaleRange / 10.0);
        base = 10.0;
    }

    double boxBegin_px, boxSpan_px;
    switch (layout) {
        case ScaleLayout::normal_vertical: {
            boxBegin_px = rc.y + rc.height;
            boxSpan_px = -((double)rc.height);
        } break;

        case ScaleLayout::normal_horizontal:
        default: {
            boxBegin_px = rc.x;
            boxSpan_px = +((double)rc.width);
        } break;
    }

    double factor = boxSpan_px / (double)scaleRange;
    double step = (scaleRange > 1.0) ? (scaleRange / stepping) : (stepping / scaleRange);
    double fine_step = step / base;
    double px = step * factor;
    double pxf = fine_step * factor;
    for (int n = 0; (n < 4) && (px < maxTextWidth_px) && (pxf < 8); n++) {
        stepping = stepping / 2.0;
        step = scaleRange / stepping;
        fine_step = step / base;

        px = step * factor;
        pxf = fine_step * factor;
    }

    double half_step = step / 2.0;
    double correction = fine_step / 1000000.0;
    double sumFullStep = step;
    double sumHalfStep = half_step;
    ScaleStep scaleStep;
    for (double div = 0.0; div <= (scaleRange + correction); div += fine_step, sumHalfStep += fine_step, sumFullStep += fine_step) {
        if ((sumHalfStep + correction) >= half_step) {
            sumHalfStep -= half_step;
            scaleStep.f_half_divider = 1;
        } else {
            scaleStep.f_half_divider = 0;
        }

        if ((sumFullStep + correction) >= step) {
            sumFullStep -= step;
            scaleStep.f_main_divider = 1;
        } else {
            scaleStep.f_main_divider = 0;
        }

        scaleStep.value = get_zoom_begin() + div;
        scaleStep.pos = (div * factor) + boxBegin_px;
        dividers.push_back(scaleStep);
    }
}

void ScaleDrawing::draw_scale_horizontal(ScalePointerType type,
    cairo_t* cr, RectEx *rc_area, RectEx *rc_scale, ColorRef color_background, ColorRef color_foreground,
    ColorRef color_pointer, const char *headline, std::vector<ScaleStep> &dividers) {

    cairo_save(cr);

    RectEx rcClip(rc_area);
    rcClip.expand(4.0);
    rcClip.clip(cr);

    cairo_translate(cr, rc_area->x, rc_area->y);
    RectEx rc(rc_scale);
    rc.x -= rc_area->x;
    rc.y -= rc_area->y;

    int overhead = (int)(rc_area->x2() - rc_scale->x2()) / 2;

    _cairo_set_source_rgb(cr, color_background);
    cairo_paint(cr);

    int fontSize = (int)(rc.height * 0.125);
    cairo_set_font_size(cr, fontSize);
    cairo_select_font_face(cr, DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    ScaleFormat format(get_format());
    format.set_scalemode(true);
    char string[256]{0};
    format_value(format, string, sizeof(string), get_top(), 0, false);

    cairo_text_extents_t extents{0};
    cairo_text_extents(cr, string, &extents);
    int maxTextWidth_px = extents.width + 8;

    calculate_for(ScaleLayout::normal_horizontal, rc, maxTextWidth_px, dividers);

    PointF pointer((double)get_normalized(), get_value());

    double y = (double)(rc.y + rc.height);
    double y1 = y;
    double y2 = y - ((double)rc.height * 0.16);
    double y3 = y - ((double)rc.height * 0.22);
    double y4 = y - ((double)rc.height * 0.30);

    switch (type) {
        case ScalePointerType::bar: {
            draw_bar_pointer_horizontal(cr, pointer, rc, color_pointer);
        } break;

        case ScalePointerType::fill: {
            draw_fill_pointer_horizontal(cr, pointer, rc, color_pointer);
        } break;

        case ScalePointerType::pointer:
        case ScalePointerType::line:
        case ScalePointerType::none:
        default: {
        } break;
    }

    int xMin = rc.x;
    int xMax = rc.x2();

    _cairo_set_source_rgb(cr, color_foreground);

    cairo_set_line_width(cr, 0.5);
    for (ScaleStep &step : dividers) {
        cairo_move_to(cr, step.pos, y1);
        if (step.f_main_divider == 1) {
            cairo_line_to(cr, step.pos, y4 + 5);

            format_value(format, string, sizeof(string), step.value, 0, false);
            cairo_text_extents(cr, string, &extents);

            int x = std::max(overhead, (int)step.pos - (int)(extents.width / 2));
            xMin = std::min(xMin, x);

            xMax = std::min((int)rc.x2() + overhead, (int)std::max(xMax, (int)(x + extents.width)));
            if ((x + extents.width) > xMax) {
                x = xMax - extents.width;
            }

            cairo_move_to(cr, x, y4);
            cairo_show_text(cr, string);
        } else if (step.f_half_divider == 1) {
            cairo_line_to(cr, step.pos, y3);
        } else {
            cairo_line_to(cr, step.pos, y2);
        }
    }
    cairo_stroke(cr);

    // @todo: Draw horizontal markers here.

    if (headline == nullptr) {
        headline = " ";
    }
    cairo_select_font_face(cr, DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_text_extents(cr, headline, &extents);
    y = rc.y + 4 + extents.height;
    _cairo_set_source_rgb(cr, color_foreground);
    cairo_move_to(cr, rc.center_x() - extents.width / 2, y);
    cairo_show_text(cr, headline);

    y = y4 - extents.height - 18;

    int largeFontSize = (int)((float)fontSize * 1.6);
    cairo_set_font_size(cr, largeFontSize);

    const char *name = get_name();
    cairo_text_extents(cr, name, &extents);
    cairo_move_to(cr, xMin, y);
    cairo_show_text(cr, name);

    char value_string[64]{0};
    format_value(get_format(), value_string, sizeof (value_string), get_value(), 0, true);

    cairo_text_extents(cr, value_string, &extents);
    cairo_move_to(cr, xMax - extents.width, y);
    cairo_show_text(cr, value_string);

    switch (type) {
        case ScalePointerType::pointer: {
            draw_regular_pointer_horizontal(cr, pointer, rc, color_pointer);
        } break;

        case ScalePointerType::line: {
            draw_line_pointer_horizontal(cr, pointer, rc, color_pointer);
        } break;

        case ScalePointerType::bar:
        case ScalePointerType::fill:
        case ScalePointerType::none:
        default: {
        } break;
    }

    cairo_restore(cr);
}

size_t ScaleDrawing::format_value(const ScaleFormat& _used_format, char* _buffer, size_t _size, double _value, int _stride, bool _with_unit) {
    if ((_buffer == nullptr) || (_size < 3)) {
        return (0);
    }

    ScaleFormat temp_format(_used_format);
    size_t nCount = (size_t)temp_format.get_number_len(top, bottom);
    size_t nColon = (size_t)temp_format.get_decimal_digits();
    if (temp_format.is_scalemode()) {
        if (nColon > 0) {
            nColon -= 1;
        }
    }

    if (_value < 0.0) {
        temp_format.set_sign(true);
    } else {
        if ((get_bottom() < 0.0f) || (get_top() < 0.0f) || (get_zoom_begin() < 0.0f) || (get_zoom_end() < 0.0f)) {
            temp_format.set_sign(true);
        }
    }

    char szFormat[32]{0};
    if (temp_format.is_sign()) {
        snprintf(szFormat, sizeof(szFormat), "%%+%d.%df", (int)(nCount - nColon), (int)nColon);
    } else {
        snprintf(szFormat, sizeof(szFormat), "%c%%%d.%df", (_stride > 0) ? '_' : ' ', (int)(nCount - nColon), (int)nColon);
    }

    char szNumber[32];
    memset(szNumber, ' ', sizeof (szNumber));
    szNumber[31] = 0;
    snprintf(&szNumber[16], 16, szFormat, _value);

    if ((temp_format.is_scalemode()) && (nColon > 1)) {
        for (size_t i = strlen(szNumber) - 1;
            (i > nColon) && (szNumber[i] == '0') && (szNumber[i - 1] != '.') && (szNumber[i - 1] != ','); i--) {
            szNumber[i] = 0;
            nColon--;
        }
    }

    nCount -= nColon;
    if ((nColon > 0) && !temp_format.is_sign()) {
        nCount--;
    }
    if (nCount < 1) {
        nCount = 1;
    }

    size_t i = 0;
    while ((i < sizeof(szNumber)) && (szNumber[i] <= ' ')) {
        i++;
    }

    if (_stride > 0) {
        i = (i / _stride) * _stride;
    }

    size_t length = 0;
    if (_with_unit) {
        const unsigned char shortSpace[4]{0xE2, 0x80, 0x89, 0x00};
        length = snprintf(_buffer, _size, "%s%s%s", &szNumber[i], (const char *)shortSpace, get_unit());
    } else {
        length = snprintf(_buffer, _size, "%s", &szNumber[i]);
    }

    return (length);
}

size_t ScaleDrawing::format_value(char* _buffer, size_t _size, double _value, bool _with_unit) {
    return (format_value(get_format(), _buffer, _size, _value, 0, _with_unit));
}

size_t ScaleDrawing::format_value(char* _buffer, size_t _size, bool _with_unit) {
    return (format_value(get_format(), _buffer, _size, get_value(), 0, _with_unit));
}

bool ScaleDrawing::get_update_request(void) {
    return (update_request);
}

void ScaleDrawing::set_update_request(bool _state) {
    update_request = _state;
}

void ScaleDrawing::clr_update_request(void) {
    set_update_request(false);
}

