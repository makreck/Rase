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

#define SIDEBAR_WIDGET_DEFAULT_WIDTH  (24)
#define SIDEBAR_WIDGET_DEFAULT_HEIGHT (136)
#define FONTSIZE_SENSOR_NAME          (12)
#define FONTSIZE_INDEX_NUMBER         (12)

class SidebarWidget {
    private:
        struct {
            SensorConnection* sensor = nullptr;
            
            GtkWidget* area = nullptr;
            cairo_surface_t* surface = nullptr;
            cairo_pattern_t* bkgPattern = nullptr;

            bool fRequestRedraw = true;
            int index_number = 0;

            RectEx rc;
            ColorRef textColor = RGBA(255, 255, 255, 255);
            ColorRef baseColor = RGBA(223, 159, 159, 255);
        } m;

        void init(SensorConnection* sensor, int width, int height);
        void cleanup(void);

        static gboolean _configure(GtkWidget* widget, cairo_t* hdc, gpointer data);
        void configure(GtkWidget* widget, cairo_t* hdc);

        static gboolean _draw_function(GtkWidget* widget, cairo_t* hdc, gpointer data);
        void draw_function(GtkWidget* widget, cairo_t* hdc);
        
        void redraw(void);
        void free_resource(void);

    public:
        SidebarWidget(SensorConnection* sensor, int width = SIDEBAR_WIDGET_DEFAULT_WIDTH, int height = SIDEBAR_WIDGET_DEFAULT_HEIGHT) {
            init(sensor, width, height);
        }

        ~SidebarWidget(void) {
            cleanup();
        }

        GtkWidget* get(void) { return (m.area); }
        void update(void);
        void set_sidebar_number(int n);
        int get_width(void) { return (m.rc.width); }
};
