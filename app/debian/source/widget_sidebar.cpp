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

void SidebarWidget::init(SensorConnection* sensor, int width, int height) {
    m.sensor = sensor;

    m.rc.set(0, 0, width, height);

    m.area = gtk_drawing_area_new();
    gtk_widget_set_size_request(m.area, m.rc.width, m.rc.height);
    
    g_signal_connect(m.area, "draw", G_CALLBACK(SidebarWidget::_draw_function), this);
    g_signal_connect(m.area, "configure-event", G_CALLBACK(SidebarWidget::_configure), this);

    update();
}

void SidebarWidget::cleanup(void) {
    free_resource();
}

void SidebarWidget::free_resource(void) {
    if (m.surface != nullptr) {
        cairo_surface_destroy(m.surface);
        m.surface = nullptr;
    }

    if (m.bkgPattern != nullptr) {
        cairo_pattern_destroy(m.bkgPattern);
        m.bkgPattern = nullptr;
    }
}

gboolean SidebarWidget::_configure(GtkWidget* _widget, cairo_t* _hdc, gpointer _data) {
    (reinterpret_cast<SidebarWidget*>(_data))->configure(_widget, _hdc);
    return (false);
}
void SidebarWidget::configure(GtkWidget* widget, cairo_t* cr) {
    free_resource();

    m.rc.set(widget);

    ColorRGBA sc(RGBA(159, 187, 223, 255)); // todo: use color from device

    m.baseColor = sc.get();
    m.textColor = sc.get_text_overlay_color();

    const std::vector<PointF> bkgPattern = { { 0.00f, 0.9f }, { 0.05f, 0.9f }, { 0.70f, 1.1f }, { 0.95f, 0.9f }, { 1.00f, 0.20f } };
    m.bkgPattern = GtkTool::make_gradientfill_pattern(GradientMode::horizontal, m.baseColor, &m.rc, bkgPattern);

    m.surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, m.rc.width, m.rc.height);

    redraw();
}

gboolean SidebarWidget::_draw_function(GtkWidget* area, cairo_t* hdc, gpointer data) {
    OBJ_PTR(SidebarWidget, data)->draw_function(area, hdc);
    return (false);
}
void SidebarWidget::draw_function(GtkWidget* area, cairo_t* hdc) {
    if (m.surface != nullptr) {
        if (m.fRequestRedraw) {
            m.fRequestRedraw = false;
            redraw();
        }
        cairo_set_source_surface(hdc, m.surface, 0, 0);
        cairo_paint(hdc);
    }
}

void SidebarWidget::redraw(void) {
    if (m.surface == nullptr) return;

    cairo_t* hdc = cairo_create(m.surface);

    const char* text = m.sensor->get_path();
    cairo_text_extents_t extents{ 0 };

    cairo_set_source(hdc, m.bkgPattern);
    cairo_rectangle(hdc, m.rc.x, m.rc.y, m.rc.width, m.rc.height);
    cairo_fill(hdc);  

    cairo_select_font_face(hdc, DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(hdc, FONTSIZE_SENSOR_NAME);
    cairo_set_source_rgb(hdc, CR_RED(m.textColor), CR_GREEN(m.textColor), CR_BLUE(m.textColor));
    cairo_text_extents(hdc, text, &extents);

    cairo_move_to(hdc, m.rc.center_x() + extents.height / 2, m.rc.y + m.rc.height - 4);
    cairo_rotate(hdc, -M_PI_2);
    cairo_show_text(hdc, text);
    cairo_rotate(hdc, +M_PI_2);

    if (m.index_number != 0) {
        char number[8]{ 0 };
        snprintf(number, sizeof (number) - 1, "%d", m.index_number);

        RectEx rcNum(m.rc);
        rcNum.height = (int)(FONTSIZE_INDEX_NUMBER * 1.5);
        rcNum.x     += 2;
        rcNum.y     += 2;
        rcNum.width -= 4;

        rcNum.fill(hdc, C_BLACK);

        cairo_select_font_face(hdc, DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(hdc, FONTSIZE_INDEX_NUMBER);
        cairo_set_source_rgb(hdc, CR_RED(C_WHITE), CR_GREEN(C_WHITE), CR_BLUE(C_WHITE));
        rcNum.center_text(hdc, number);
    }

    cairo_destroy(hdc);
}

void SidebarWidget::update(void) {
    if (GTK_IS_WIDGET(m.area)) {
        m.fRequestRedraw = true;
        gtk_widget_queue_draw(m.area);
    }
}

void SidebarWidget::set_sidebar_number(int n) {
    m.index_number = std::max(0, std::min(99, n));
    update();
}
