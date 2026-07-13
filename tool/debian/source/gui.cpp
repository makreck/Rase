/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,      Preliminary Configuration Tool
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

const char* svg_app    = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\"> <rect x=\"3\" y=\"3\" width=\"18\" height=\"18\" rx=\"2\" ry=\"2\"/><line x1=\"3\" y1=\"12\" x2=\"21\" y2=\"12\"/><line x1=\"12\" y1=\"3\" x2=\"12\" y2=\"21\"/></svg>\n";
const char* svg_search = "<svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"><path fill-rule=\"evenodd\" d=\"M16.3198574,14.9056439 L21.7071068,20.2928932 L20.2928932,21.7071068 L14.9056439,16.3198574 C13.5509601,17.3729184 11.8487115,18 10,18 C5.581722,18 2,14.418278 2,10 C2,5.581722 5.581722,2 10,2 C14.418278,2 18,5.581722 18,10 C18,11.8487115 17.3729184,13.5509601 16.3198574,14.9056439 Z M10,16 C13.3137085,16 16,13.3137085 16,10 C16,6.6862915 13.3137085,4 10,4 C6.6862915,4 4,6.6862915 4,10 C4,13.3137085 6.6862915,16 10,16 Z\"/></svg>";

const ToolbarItems mainToolbar[] = {
    { svg_search, (void*)IDS_DEVICE_SCAN },
};
const size_t sizeOf_mainToolbar = SIZEOFARRAY(mainToolbar);

void App::run_gui(void) {
    gtk_init(&m.argc, &m.argv);
    m.gtkApp = gtk_application_new(nullptr, APP_FLAGS);
    g_signal_connect(m.gtkApp, "activate", G_CALLBACK(App::_activate), this);
    g_application_run(G_APPLICATION(m.gtkApp), m.argc, m.argv);
}

gboolean App::_activate(GtkApplication* gtk, void* user_data) {
    APP_PTR(user_data)->activate();
    return (false);
}
void App::activate(void) {
    find_interface();
    create_app_window();
    gtk_main();
}

void App::get_main_window_placing(void) {
    m.rc.client.x      = 64;
    m.rc.client.y      = 64;
    m.rc.client.width  = APP_WINDOW_MIN_WIDTH;
    m.rc.client.height = APP_WINDOW_MIN_HEIGHT;

    m.gtk.win = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GdkGeometry hints{ 0 };
    hints.min_width   = APP_WINDOW_MIN_WIDTH;
    hints.min_height  = APP_WINDOW_MIN_HEIGHT;
    hints.max_width   = APP_WINDOW_MAX_WIDTH;
    hints.max_height  = APP_WINDOW_MAX_HEIGHT;
    hints.base_width  = m.rc.client.width;
    hints.base_height = m.rc.client.height;
    hints.width_inc   = 8;
    hints.height_inc  = 8;
    hints.min_aspect  = 1.0;
    hints.max_aspect  = 1.0;
    hints.win_gravity = GdkGravity::GDK_GRAVITY_CENTER;

    gtk_window_set_geometry_hints(GTK_WINDOW(m.gtk.win), GTK_WIDGET(m.gtk.win), &hints, (GdkWindowHints)(GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE));
    gtk_window_set_default_size(GTK_WINDOW(m.gtk.win), m.rc.client.width, m.rc.client.height);
    gtk_window_set_resizable(GTK_WINDOW(m.gtk.win), false);
    gtk_window_set_position(GTK_WINDOW(m.gtk.win), GTK_WIN_POS_CENTER);
}

void App::create_app_window(void) {
    get_main_window_placing();

    GdkPixbuf* icon = App::svg2image(svg_app, 64, 64, C_WHITE);
    if (icon != nullptr) {
        gtk_window_set_icon((GtkWindow*)m.gtk.win, icon);
    }

    gtk_container_set_border_width(GTK_CONTAINER(m.gtk.win), 4);

    gtk_window_set_title(GTK_WINDOW(m.gtk.win), APP_WINDOW_NAME);
    gtk_window_set_default_size(GTK_WINDOW(m.gtk.win), m.rc.client.width, m.rc.client.height);
    gtk_window_set_position(GTK_WINDOW(m.gtk.win), GTK_WIN_POS_CENTER);

    create_layout();
    set_main_window_callbacks();

    gtk_widget_show_all(m.gtk.win);
    gtk_window_present(GTK_WINDOW(m.gtk.win));
}

void App::set_main_window_callbacks(void) {
    g_signal_connect(m.gtk.win, "destroy",         G_CALLBACK(gtk_main_quit),   nullptr);
    g_signal_connect(m.gtk.win, "configure-event", G_CALLBACK(App::_configure), this);
    g_signal_connect(m.gtk.win, "realize",         G_CALLBACK(App::_realize),   this);
}

gboolean App::_configure(GtkWindow* parentWindow, GdkEvent* event, void* user_data) {
    APP_PTR(user_data)->configure(event);
    return (true);
}
void App::configure(GdkEvent* event) {
    if (event->type == GdkEventType::GDK_CONFIGURE) {
        GdkEventConfigure* ev = (GdkEventConfigure*)event;
        on_move_or_size((int)ev->x, (int)ev->y, (int)ev->width, (int)ev->height);
    }
}

gboolean App::_realize(GtkWidget* widget, void* user_data) {
    APP_PTR(user_data)->realize(widget);
    return (true);
}
void App::realize(GtkWidget* widget) {
}

void App::on_move_or_size(int x, int y, int width, int height) {
    m.rc.client.x = x;
    m.rc.client.y = y;
    m.rc.client.width = width;
    m.rc.client.height = height;
}

void App::create_layout(void) {
    m.gtk.baseVBox = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(m.gtk.win), m.gtk.baseVBox);

    gtk_box_pack_start(GTK_BOX(m.gtk.baseVBox), create_main_menu(),    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(m.gtk.baseVBox), create_main_toolbar(), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(m.gtk.baseVBox), create_dialog(),       TRUE,  TRUE,  0);
    gtk_box_pack_end(GTK_BOX(m.gtk.baseVBox),   create_statusbar(),    FALSE, FALSE, 0);
}

GdkPixbuf* App::svg2image(const char* svg_string, int width_px, int height_px, ColorRef color) {
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

GtkWidget* App::create_toolbar(const ToolbarItems* itemList, size_t itemListSize,
    const char** stringList, size_t stringListSize, int iconSize_px, GCallback cb, void* parameter) {

    if ((itemList == nullptr) || (itemListSize < 1) || (itemListSize > TOOLBAR_BUTTON_COUNT_MAX)) {
        return (nullptr);
    }

    GtkToolbar* toolbar = (GtkToolbar*)gtk_toolbar_new();
    gtk_toolbar_set_style(toolbar, GtkToolbarStyle::GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(toolbar, GtkIconSize::GTK_ICON_SIZE_LARGE_TOOLBAR);

    for (size_t i = 0; i < itemListSize; i++) {
        GdkPixbuf* pixbuf = App::svg2image(itemList[i].svg, iconSize_px, iconSize_px, C_WHITE);        
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

GtkWidget* App::create_main_menu(void) {
    m.gtk.menuBar = gtk_menu_bar_new();

    { // file menu
        GtkWidget* fileMenu = gtk_menu_new();
        GtkWidget* fileMi = gtk_menu_item_new_with_label(APPSTRING(IDS_FILE));
        gtk_menu_shell_append(GTK_MENU_SHELL(m.gtk.menuBar), fileMi);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);

        GtkWidget* quitMi = gtk_menu_item_new_with_label(APPSTRING(IDS_QUIT));
        g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(App::_on_command), ON_ITEM(IDS_QUIT));
        gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);
    }

    { // edit menu
        GtkWidget* editMenu = gtk_menu_new();
        GtkWidget* editMi = gtk_menu_item_new_with_label(APPSTRING(IDS_EDIT));
        gtk_menu_shell_append(GTK_MENU_SHELL(m.gtk.menuBar), editMi);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(editMi), editMenu);

        GtkWidget* copyMi = gtk_menu_item_new_with_label(APPSTRING(IDS_COPY));
        g_signal_connect(G_OBJECT(copyMi), "activate", G_CALLBACK(App::_on_command), ON_ITEM(IDS_COPY));
        gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), copyMi);

        GtkWidget* pasteMi = gtk_menu_item_new_with_label(APPSTRING(IDS_PASTE));
        g_signal_connect(G_OBJECT(pasteMi), "activate", G_CALLBACK(App::_on_command), ON_ITEM(IDS_PASTE));
        gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), pasteMi);
    }

    return (m.gtk.menuBar);
}

GtkWidget* App::create_main_toolbar(void) {
    m.gtk.toolbar = App::create_toolbar(mainToolbar, sizeOf_mainToolbar, 
                                        &app_strings_main[APPLANG][0], IDS_MAIN_COUNT, 
                                        m.toolIconSize, G_CALLBACK(App::_on_command), this);
    gtk_widget_set_hexpand(m.gtk.toolbar, TRUE);
    return (m.gtk.toolbar);
}

void App::_on_command(GtkApplication* gtk, void* callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(callback_parameter);
    OBJ_PTR(App, cbp->get_this())->on_command(cbp);
}
void App::on_command(CallbackParameter* p) {
    int64_t itemID = (int64_t)(p->get_pointer());

    switch (itemID) {
        case IDS_QUIT: {
            gtk_main_quit();
        } break;

        case IDS_COPY: {
        } break;

        case IDS_PASTE: {
        } break;

        case IDS_DEVICE_SCAN: {
        } break;

        default: {
        } break;
    }
}

GtkWidget* App::create_dialog(void) {
    m.gtk.dialog = gtk_frame_new("Dialog");
    gtk_frame_set_shadow_type(GTK_FRAME(m.gtk.dialog), GTK_SHADOW_IN);
    return (m.gtk.dialog);
}

GtkWidget* App::create_statusbar(void) {
    m.gtk.statusBar = gtk_label_new("Status bar.");
    gtk_label_set_xalign(GTK_LABEL(m.gtk.statusBar), 0.0f);
    gtk_widget_set_hexpand(m.gtk.statusBar, TRUE);
    gtk_frame_set_shadow_type(GTK_FRAME(m.gtk.statusBar), GTK_SHADOW_IN);
    gtk_widget_set_size_request(m.gtk.statusBar, -1, 28);

    snprintf(m.status_text, sizeof (m.status_text), "Connected: \"%s\" ", m.ifac);
    status_update();

    return (m.gtk.statusBar);
}

void App::status_update(const char* _string) {
    if (_string != nullptr) {
        memset(m.status_text, 0, sizeof (m.status_text));
        strncpy(m.status_text, _string, sizeof (m.status_text) - 1);
    }
    gtk_label_set_text(GTK_LABEL(m.gtk.statusBar), m.status_text);
}
