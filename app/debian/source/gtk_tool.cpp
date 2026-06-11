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

GtkWidget* GtkTool::create_toolbar(const ToolbarItems* itemList, size_t itemListSize,
    const char** stringList, size_t stringListSize, int iconSize_px, GCallback cb, void* parameter) {

    if ((itemList == nullptr) || (itemListSize < 1) || (itemListSize > TOOLBAR_BUTTON_COUNT_MAX)) {
        return (nullptr);
    }

    GtkToolbar* toolbar = (GtkToolbar*)gtk_toolbar_new();
    gtk_toolbar_set_style(toolbar, GtkToolbarStyle::GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(toolbar, GtkIconSize::GTK_ICON_SIZE_LARGE_TOOLBAR);

    for (size_t i = 0; i < itemListSize; i++) {
        GdkPixbuf* pixbuf = GtkTool::svg2image(itemList[i].svg, iconSize_px, iconSize_px, C_WHITE);        
        if (pixbuf != nullptr) {
            GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
            g_object_unref(pixbuf);
            const char* name = nullptr;
            if ((uint64_t)(itemList[i].text_id) > 255) {
                name = (const char*)itemList[i].text_id;
            } else {
                int stringIndex = (int)(((uint64_t)itemList[i].text_id) & 0xff);
                if ((stringIndex > 0) && (stringIndex < stringListSize)) {
                    name = stringList[stringIndex];
                }
            }

            GtkToolItem* item = (GtkToolItem *)gtk_tool_button_new(image, name);
            gtk_tool_item_set_tooltip_text(item, name);

            if (cb != nullptr) {
                g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(cb), new CallbackParameter(parameter, (void*)itemList[i].text_id));
            }

            gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, -1);
        }
    }

    return ((GtkWidget*)toolbar);
}

GtkWidget* GtkTool::create_empty_pixbuf(int width_px, int height_px, ColorRef background_color, gboolean has_alpha) {
    GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, has_alpha, 8, width_px, height_px);
    if (pixbuf != nullptr) {
        gdk_pixbuf_fill(pixbuf, background_color);
    }
    return ((GtkWidget*)pixbuf);
}

GdkPixbuf* GtkTool::svg2image(const char* svg_string, int width_px, int height_px, ColorRef color) {
    if (svg_string == nullptr) {
        return (nullptr);
    }

    size_t length = strlen(svg_string);
    char* modified_string = (char*)malloc(length + 8);
    memcpy(modified_string, svg_string, length + 1);
    modified_string[length] = 0;

    char color_str[16]{ 0 };
    snprintf(color_str, sizeof (color_str), "#%-2.2X%-2.2X%-2.2X", RGBA_R(color), RGBA_G(color), RGBA_B(color));
    size_t len_c = std::min(strlen(color_str), strlen(SVG_STROKE_COLOR));

    char* p = modified_string;
    do {
        p = strstr(p, SVG_STROKE_COLOR);
        if (p != nullptr) {
            memcpy(p, color_str, len_c);
            p += len_c;
        }
    } while (p != nullptr);

    GError* error = nullptr;
    RsvgHandle* svg_handle = rsvg_handle_new_from_data((const guint8 *)modified_string, length, &error);
    if (error != nullptr) {
        return (nullptr);
    }

    RsvgRectangle viewport = { 0.0, 0.0, (double)width_px, (double)width_px, };
    GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, width_px, height_px);
    cairo_surface_t* surface = cairo_image_surface_create_for_data(gdk_pixbuf_get_pixels(pixbuf), CAIRO_FORMAT_ARGB32, width_px, height_px, gdk_pixbuf_get_rowstride(pixbuf));
    cairo_t* cr = cairo_create(surface);

    cairo_translate(cr, 1.0, 1.0);
    cairo_scale(cr, 1.0, 1.0);
    cairo_set_source_rgba(cr, CR_RED(color), CR_GREEN(color), CR_BLUE(color), CR_ALPHA(color));
    rsvg_handle_render_document(svg_handle, cr, &viewport, &error);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(svg_handle);

    
    free(modified_string);

    return (pixbuf);
}

void GtkTool::set_event_list(GtkWidget* widget, const std::vector<const char*>& list, GCallback handler, void* object) {
    if ((widget != nullptr) && (handler != nullptr) && (object != nullptr)) {
        for (const char* eventName : list) {
            g_signal_connect(widget, eventName, G_CALLBACK(handler), (gpointer)object);
        }
        gtk_widget_set_events(widget, GdkEventMask::GDK_ALL_EVENTS_MASK); 
    }
}

void GtkTool::set_mouse_button_event_list(GtkWidget* widget, GCallback handler, void* object) {
    const std::vector<const char *> mouseButtonStdEventList = {
        "button-press-event",
        "button-release-event",
        "motion-notify-event",
        "enter-notify-event",
        "leave-notify-event",
        "scroll-event",
    };
    GtkTool::set_event_list(widget, mouseButtonStdEventList, handler, object);
}

cairo_pattern_t* GtkTool::create_pattern(GradientMode _mode, RectEx* _rc) {
    cairo_pattern_t* pattern = nullptr;
    if (_mode == GradientMode::horizontal) {
        pattern = cairo_pattern_create_linear(0.0, 0.0, _rc->width, 0.0);
    } else if (_mode == GradientMode::vertical) {
        pattern = cairo_pattern_create_linear(0.0, 0.0, 0.0, _rc->height);
    } else if (_mode == GradientMode::diagonal) {
        pattern = cairo_pattern_create_linear(0.0, 0.0, _rc->width, _rc->height);
    }
    return (pattern);
}

void GtkTool::delete_pattern(cairo_pattern_t*& _pattern) {
    if (_pattern != nullptr) {
        cairo_pattern_destroy(_pattern);
        _pattern = nullptr;
    }
}

cairo_pattern_t* GtkTool::make_gradientfill_pattern(GradientMode _mode, ColorRef _color, RectEx* _rc, const std::vector<PointF>& steps) {
    cairo_pattern_t* pattern = GtkTool::create_pattern(_mode, _rc);

    ColorRGBA color_rgba(_color);
    for (const PointF& pt : steps) {
        ColorRef c = color_rgba.mul(pt.value);
        cairo_pattern_add_color_stop_rgb(pattern, pt.pos, CR_RED(c), CR_GREEN(c), CR_BLUE(c));
    }

    return (pattern);
}

cairo_pattern_t* GtkTool::make_gradientfill_pattern(GradientMode _mode, ColorRef _colorRefFrom, ColorRef _colorRefTo, RectEx* _rc) {
    cairo_pattern_t* pattern = GtkTool::create_pattern(_mode, _rc);
    cairo_pattern_add_color_stop_rgb(pattern, 0.0, CR_RED(_colorRefFrom), CR_GREEN(_colorRefFrom), CR_BLUE(_colorRefFrom));
    cairo_pattern_add_color_stop_rgb(pattern, 1.0, CR_RED(_colorRefTo), CR_GREEN(_colorRefTo), CR_BLUE(_colorRefTo));
    return (pattern);
}

cairo_pattern_t* GtkTool::make_gradientfill_pattern(GradientMode _mode, std::vector<ColorRef> _c, RectEx* _rc) {
    cairo_pattern_t* pattern = GtkTool::create_pattern(_mode, _rc);

    size_t n = (size_t)_c.size();
    if (n > 0) {
        for (size_t i = 0; i < n; i++) {
            double f = (double)i / (double)(n - 1);
            ColorRef cr = _c[i];
            cairo_pattern_add_color_stop_rgb(pattern, f, CR_RED(cr), CR_GREEN(cr), CR_BLUE(cr));
        }
    }

    return (pattern);
}
