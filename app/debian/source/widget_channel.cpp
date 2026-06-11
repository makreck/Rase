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

class ChannelWidgetKeyIcon {
    public:
        const char* svg_string;
        const char* key_string;
};

ChannelWidgetKeyIcon channel_icons[] = {
    { svg_default,      "default"      },

    { svg_air_pressure, "air_pressure" },
    { svg_rel_humidity, "rel_humidity" },
    { svg_temperature,  "temperature"  },
    { svg_dewpoint,     "dewpoint"     },
    { svg_abs_humidity, "abs_humidity" },

    { svg_generic,      "generic"      },
    { svg_percent,      "percent"      },
    { svg_wind,         "wind"         },
    { svg_voltage,      "voltage"      },
};

void ChannelWidget::init(int _channel_index, int _width, int _height, Scale *_channel_data, SensorWidget *_parent) {
    m.base = _parent;
    m.channel_index = _channel_index;
    m.channel_data = (ScaleDrawing*)_channel_data;
    m.channel_widget = nullptr;

    if (_width < 0) {
        _width = CHANNEL_WIDGET_DEFAULT_WIDTH;
    }

    if (_height < 0) {
        _height = CHANNEL_WIDGET_DEFAULT_HEIGHT;
    }

    m.rc.set(0, 0, std::max(1, _width), std::max(1, _height));

    m.area = gtk_drawing_area_new();
    gtk_widget_set_size_request(m.area, m.rc.width, m.rc.height);

    g_signal_connect(m.area, "configure-event", G_CALLBACK(ChannelWidget::_configure), this);
    g_signal_connect(m.area, "draw", G_CALLBACK(ChannelWidget::_draw_function), this);

    m.icon = create_icon_from_key(m.channel_data->get_key());
}

GtkWidget* ChannelWidget::create_icon_from_key(const char* key_string) {
    const char* svg = channel_icons[0].svg_string;
    for (int i = 0; i < SIZEOFARRAY(channel_icons); i++) {
        if (strcmp(channel_icons[i].key_string, key_string) == 0) {
            svg = channel_icons[i].svg_string;
            break;
        }
    }
    return ((GtkWidget*)GtkTool::svg2image(svg, 32, 32, RGBA(0, 0, 0, 255)));
}

void ChannelWidget::cleanup(void) {
    if (m.icon != nullptr) {
        g_object_unref(m.icon);
        m.icon = nullptr;
    }
}

gboolean ChannelWidget::_configure(GtkWidget* _widget, cairo_t* _cr, gpointer _data) {
    OBJ_PTR(ChannelWidget, _data)->configure(_widget, _cr);
    return (false);
}
void ChannelWidget::configure(GtkWidget* _widget, cairo_t* _cr) {
    if (m.surface != nullptr) {
        cairo_surface_destroy(m.surface);
    }

    m.rc.set(_widget);

    GtkTool::delete_pattern(m.bkgPattern);
    GtkTool::delete_pattern(m.hilightedPattern);
    GtkTool::delete_pattern(m.selectedPattern);

    const std::vector<PointF> background_pattern = { { 0.00f, 0.25f }, { 0.05f, 0.75f }, { 0.70f, 0.95f }, { 0.95f, 1.00f }, { 1.00f, 0.20f } };
    m.bkgPattern       = GtkTool::make_gradientfill_pattern(GradientMode::vertical, RGBA(191, 203, 239, 255), &m.rc, background_pattern);
    m.hilightedPattern = GtkTool::make_gradientfill_pattern(GradientMode::vertical, RGBA(159, 203, 255, 255), &m.rc, background_pattern);
    m.selectedPattern  = GtkTool::make_gradientfill_pattern(GradientMode::vertical, RGBA(255, 255, 255, 255), &m.rc, background_pattern);

    m.surface = gdk_window_create_similar_surface(gtk_widget_get_window(_widget), CAIRO_CONTENT_COLOR, m.rc.width, m.rc.height);

    update();
}

SensorWidget* ChannelWidget::get_parent(void) {
    return (m.base);
}

int ChannelWidget::get_channel_index(void) {
    return (m.channel_index);
}

GtkWidget* ChannelWidget::get_widget(void) {
    return (m.area);
}

int ChannelWidget::get_width(void) {
    return ((m.channel_widget != nullptr) ? (m.channel_widget->get_width()) : 0);
}

void ChannelWidget::clear_channel_state(void) {
    if (m.channel_widget != nullptr) {
        m.channel_widget->set_state(false, false);
    }
}

void ChannelWidget::set_channel_state(bool _hilighted, bool _selected) {
    if (m.channel_widget != nullptr) {
        m.channel_widget->set_state(_hilighted, _selected);
    }
}

void ChannelWidget::update(void) {
    if (GTK_IS_WIDGET(m.area)) {
        m.request_redraw = true;
        gtk_widget_queue_draw(m.area);
    }
}

void ChannelWidget::set_state(bool _hilighted, bool _selected, uint8_t _flags) {
    uint32_t previous_flags = m.flags;

    if (_flags & 0x01) {
        m.hilighted = _hilighted;
    }

    if (_flags & 0x02) {
        m.selected = _selected;
    }

    if (previous_flags != m.flags) {
        update();
    }
}

void ChannelWidget::request_update(void) {
    m.request_redraw = true;
}

gboolean ChannelWidget::_draw_function(GtkWidget* _area, cairo_t* _cr, gpointer _data) {
    OBJ_PTR(ChannelWidget, _data)->draw_function(_area, _cr);
    return (false);
}
void ChannelWidget::draw_function(GtkWidget* _area, cairo_t* _cr) {
    if (m.surface != nullptr) {
        if (m.request_redraw) {
            m.request_redraw = false;
            redraw();
        }
        cairo_set_source_surface(_cr, m.surface, 0, 0);
    }
    cairo_paint(_cr);
}

void ChannelWidget::redraw(void) {
    if (m.surface == nullptr) return;

    cairo_t* cr = cairo_create(m.surface);
    cairo_text_extents_t extents{ 0 };

    if (m.selected == 1) {
        cairo_set_source(cr, m.selectedPattern);
    } else if (m.hilighted == 1) {
        cairo_set_source(cr, m.hilightedPattern);
    } else {
        cairo_set_source(cr, m.bkgPattern);
    }
    m.rc.set(cr);
    cairo_fill(cr);

    if (m.icon != nullptr) {
        cairo_save(cr);
        cairo_translate(cr, 4, 12);
        gdk_cairo_set_source_pixbuf(cr, (const GdkPixbuf*)(m.icon), 0, 0);
        cairo_paint(cr);
        cairo_restore(cr);
    }

    cairo_set_source_rgb(cr, 0, 0, 0);
    int x = m.rc.x + 48;
    int y = 8;

    cairo_set_font_size(cr, FONTSIZE_TASK_NAME);
    cairo_select_font_face(cr, DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    const char* name = m.channel_data->name;
    cairo_text_extents(cr, name, &extents);
    y += extents.height;
    cairo_move_to(cr, m.rc.center_x() - extents.width / 2, y);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, name);

    char value[32]{ 0 };
    m.channel_data->format_value(value, sizeof (value), false);
    cairo_select_font_face(cr, DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, FONTSIZE_TASK_VALUE);
    cairo_text_extents(cr, value, &extents);
    y += (8 + extents.height);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, value);
    cairo_set_font_size(cr, FONTSIZE_TASK_VALUE / 2.0);
    cairo_move_to(cr, x + extents.width + 8, y);
    cairo_show_text(cr, m.channel_data->get_unit());

    x = m.rc.x + extents.height;
    y = m.rc.y2() - CURVE_COLOR_BAR_HEIGHT - extents.height;

    RectEx rc_bar(m.rc);
    rc_bar.y = (rc_bar.y2() - (CURVE_COLOR_BAR_HEIGHT * 2.0));
    rc_bar.height = (int)CURVE_COLOR_BAR_HEIGHT;
    rc_bar.x = x;
    rc_bar.width -= (int)(extents.height * 2.0);

    m.channel_data->format_value(value, sizeof(value), m.channel_data->get_min(), false);
    cairo_text_extents(cr, value, &extents);
    cairo_move_to(cr, x + 4, y);
    cairo_show_text(cr, value);

    m.channel_data->format_value(value, sizeof(value), m.channel_data->get_average(), false);
    cairo_text_extents(cr, value, &extents);
    cairo_move_to(cr, m.rc.center_x() - extents.width / 2, y);
    cairo_show_text(cr, value);

    m.channel_data->format_value(value, sizeof(value), m.channel_data->get_max(), false);
    cairo_text_extents(cr, value, &extents);
    cairo_move_to(cr, rc_bar.x2() - extents.width - 4, y);
    cairo_show_text(cr, value);

    rc_bar.fill(cr, C_WHITE);
    rc_bar.shrink(1.0);
    rc_bar.fill(cr, m.channel_data->get_color_ref());

    cairo_destroy(cr);
}
