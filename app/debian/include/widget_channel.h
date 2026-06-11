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

class SensorWidget;

#define FONTSIZE_TASK_NAME (11)
#define FONTSIZE_TASK_VALUE (24)
#define CURVE_COLOR_BAR_HEIGHT (10)
#define CHANNEL_WIDGET_DEFAULT_WIDTH  (128)
#define CHANNEL_WIDGET_DEFAULT_HEIGHT (120)

class ChannelWidget {
    private:
        struct {
            SensorWidget*    base               = nullptr;
            ChannelWidget*   channel_widget     = nullptr;
            ScaleDrawing*    channel_data       = nullptr;
            int              channel_index      = -1;

            RectEx           rc;
            GtkWidget*       area               = nullptr;
            GtkWidget*       icon               = nullptr;

            cairo_surface_t* surface            = nullptr;
            cairo_pattern_t* bkgPattern         = nullptr;
            cairo_pattern_t* hilightedPattern   = nullptr;
            cairo_pattern_t* selectedPattern    = nullptr;

            bool request_redraw = true;

            union {
                uint32_t flags = 0;
                struct {
                    uint32_t hilighted : 1;
                    uint32_t selected  : 1;
                    uint32_t reserved1 : 6;

                    uint32_t reserved2 : 8;

                    uint32_t reserved3 : 8;

                    uint32_t reserved4 : 8;
                };
            };

        } m;

        void init(int _channel_index, int _width, int _height, Scale* _channel_data, SensorWidget* _parent);
        void cleanup(void);
        void redraw(void);

        static gboolean _draw_function(GtkWidget* _area, cairo_t* _cr, gpointer _data);
        void draw_function(GtkWidget* _area, cairo_t* _cr);

        static gboolean _configure(GtkWidget* _widget, cairo_t* _cr, gpointer _data);
        void configure(GtkWidget* _widget, cairo_t* _cr);
        
        GtkWidget* create_icon_from_key(const char* key_string);

    public:
        ChannelWidget(int _channel_index, int _width, int _height, Scale* _channel_data, SensorWidget* _parent) {
            init(_channel_index, _width, _height, _channel_data, _parent);
        }

        ~ChannelWidget() {
            cleanup();
        }

        SensorWidget* get_parent(void);
        int get_channel_index(void);
        GtkWidget* get_widget(void);
        int get_width(void);
        void clear_channel_state(void);
        void set_channel_state(bool _hilighted, bool _selected);
        void set_state(bool _hilighted, bool _selected, uint8_t _flags = 0x03);
        void request_update(void);
        void update(void);
};
