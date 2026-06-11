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

#pragma once

#define TOOLBAR_BUTTON_COUNT_MAX (64)

#define KEY_STATE_EX_NONE       (0x00000000)
#define KEY_STATE_EX_SHIFT      (0x00000001)
#define KEY_STATE_EX_CONTROL    (0x00000004)
#define KEY_STATE_EX_ALT        (0x00000080)

#define KEY_BUTTON_MOUSE_LEFT   (1)
#define KEY_BUTTON_MOUSE_MIDDLE (2)
#define KEY_BUTTON_MOUSE_RIGHT  (3)

enum class GradientMode {
    horizontal = 0,
    vertical = 1,
    diagonal = 2,
};

class GdkEventBase {
    public:
        GdkEventType type;
        GdkWindow*   window;
        gint8        send_event;
        guint32      time;
        gdouble      x;
        gdouble      y;
};

class ToolbarItems {
    public:
        const char* svg;
        const void* text_id;
};

class GtkTool {
    public:
        static GtkWidget* create_toolbar(const ToolbarItems* itemList, size_t itemListSize,
            const char** stringList, size_t stringListSize, int iconSize_px, GCallback cb, void* parameter);

        static GtkWidget* create_empty_pixbuf(int width_px, int height_px, ColorRef background_color, gboolean has_alpha = true);
        static GdkPixbuf* svg2image(const char* svg_string, int width_px, int height_px, ColorRef color);

        static void set_event_list(GtkWidget* widget, const std::vector<const char*>& list, GCallback handler, void* object);
        static void set_mouse_button_event_list(GtkWidget* widget, GCallback handler, void* object);

        static void delete_pattern(cairo_pattern_t*& _pattern);
        static cairo_pattern_t* create_pattern(GradientMode _mode, RectEx* _rc);

        static cairo_pattern_t* make_gradientfill_pattern(GradientMode _mode, ColorRef _colorRef, RectEx* _rc, const std::vector<PointF>& _steps);
        static cairo_pattern_t* make_gradientfill_pattern(GradientMode _mode, ColorRef _colorRefFrom, ColorRef _colorRefTo, RectEx* _rc);
        static cairo_pattern_t* make_gradientfill_pattern(GradientMode _mode, std::vector<ColorRef> _color, RectEx* _rc);
};
