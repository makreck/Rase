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


// GtkWidget* checkbox = gtk_check_button_new_with_label(hint);
// g_signal_connect(checkbox, "toggled", G_CALLBACK(DialogBox::_itemEvent), p);
// gtk_grid_attach(GTK_GRID(gtk.grid), checkbox, pos.x + 1, pos.y, 1, 1);

const char* svg_app    = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\"> <rect x=\"3\" y=\"3\" width=\"18\" height=\"18\" rx=\"2\" ry=\"2\"/><line x1=\"3\" y1=\"12\" x2=\"21\" y2=\"12\"/><line x1=\"12\" y1=\"3\" x2=\"12\" y2=\"21\"/></svg>\n";
const char* svg_search = "<svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"><path fill-rule=\"evenodd\" d=\"M16.3198574,14.9056439 L21.7071068,20.2928932 L20.2928932,21.7071068 L14.9056439,16.3198574 C13.5509601,17.3729184 11.8487115,18 10,18 C5.581722,18 2,14.418278 2,10 C2,5.581722 5.581722,2 10,2 C14.418278,2 18,5.581722 18,10 C18,11.8487115 17.3729184,13.5509601 16.3198574,14.9056439 Z M10,16 C13.3137085,16 16,13.3137085 16,10 C16,6.6862915 13.3137085,4 10,4 C6.6862915,4 4,6.6862915 4,10 C4,13.3137085 6.6862915,16 10,16 Z\"/></svg>";

const char* sensor_types_list = "autoscan\nNull\nSHT2x\nSHT3x\nHTU21d\nATHxx\nHDC1080\nBMx280";
const char* display_timeouts  = "Never\n10 sec.\n1 min.\n5 min.\n15 min.\n30 min.";
const char* display_contrast  = "100%\n80%\n60%\n50%\n40%\n30%\n20%\n10%";
const char* display_page      = "Value page\nDetails page\nInfo page";
const char* display_rotation  = "0°\n180°";
const char* led_intensity     = "Max. 100%\nHigh 75%\nMedium 50%\nLow 25%\nVery low 5%Min. 1%";

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
    if (find_interface()) {
        char* response = transact_command("/config");
        if (response != nullptr) {
            import_config(response);
            free(response);

            create_app_window();
            handle_dialog_items(true);
            gtk_main();
        }
    }
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
    const char** _stringList, size_t stringListSize, int iconSize_px, GCallback cb, void* parameter) {

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
                    name = _stringList[stringIndex];
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

GtkWidget* App::create_statusbar(void) {
    m.gtk.statusBar = gtk_label_new("Status bar.");
    gtk_label_set_xalign(GTK_LABEL(m.gtk.statusBar), 0.0f);
    gtk_widget_set_hexpand(m.gtk.statusBar, TRUE);
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

GtkWidget* App::add_grid(const char* _label, GtkWidget* _parent) {
    GtkWidget* frame = gtk_frame_new(_label);
    gtk_container_set_border_width(GTK_CONTAINER(frame), 8);

    GtkWidget* grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 4);
    gtk_container_add(GTK_CONTAINER(frame), grid);

    gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);

    gtk_box_pack_start(GTK_BOX(_parent), frame, FALSE, FALSE, 0);

    return (grid);
}

int App::string_combobox_setup(GtkWidget* _widget, const char* _selected, const char* _stringList) {
    if ((_widget == nullptr) || ((_selected == nullptr) && (_stringList == nullptr))) { return (0); }

    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(_widget));

    int i_sel = -1;
    int n = 0;

    if (_stringList != nullptr) {
        while(*_stringList != '\0') {
            char string[256]{ 0 };
            for (int i = 0; (i < sizeof (string) - 2) && (*_stringList != '\n')&& (*_stringList != '\0'); i++) {
                string[i + 0] = *_stringList++;
                string[i + 1] = 0;
            }
            if (*_stringList == '\n') {
                _stringList++;
            }
            
            if (strlen(string) > 0) {
                if ((_selected != nullptr) && (i_sel == -1)) {
                    if (!strcmp(string, _selected)) {
                        i_sel = n;
                    }
                }
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(_widget), string);
                n++;
            }
        }
    }

    if ((_selected != nullptr) && (i_sel == -1)) {
        if (strlen(_selected) > 0) {
            gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(_widget), 0, _selected);
            i_sel = 0;
            n++;
        }
    }

    if (i_sel != -1) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(_widget), (gint)i_sel);
    }

    return (n);
}

DialogItem* App::add_text_field(GtkWidget* _grid, int _item_id, int _width, int _x, int _y, char* _field, size_t _length, const char* _list) {
    DialogItem* item = new DialogItem(_item_id, _field, _length, _list);

    item->label = gtk_label_new(APPSTRING(_item_id));
    gtk_widget_set_size_request(item->label, APP_WINDOW_LABEL_WIDTH, -1);
    gtk_label_set_xalign(GTK_LABEL(item->label), 0.0f);
    gtk_grid_attach(GTK_GRID(_grid), item->label, _x, _y, 1, 1);

    if (_list != nullptr) {
        item->widget = gtk_combo_box_text_new();
        // App::string_combobox_setup(item->widget, _field, _list);
    } else {
        item->widget = gtk_entry_new();
        // gtk_entry_set_max_length(GTK_ENTRY(item->widget), _length);
        // gtk_entry_set_text(GTK_ENTRY(item->widget), _field);
    }

    if (_width <= 0) {
        _width = (int)_length * 8;
    }
    gtk_widget_set_size_request(item->widget, std::min(320, _width), -1);
    g_signal_connect(item->widget, "changed", G_CALLBACK(App::_on_command), ON_ITEM(_item_id));
    gtk_grid_attach(GTK_GRID(_grid), item->widget, _x + 1, _y, 1, 1);

    return (item);
}

GtkWidget* App::create_dialog(void) {
    m.gtk.dialog = gtk_frame_new(nullptr);
    gtk_frame_set_shadow_type(GTK_FRAME(m.gtk.dialog), GTK_SHADOW_IN);

    m.gtk.scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width((GtkContainer *)m.gtk.scrolled, 4);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *)m.gtk.scrolled, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(m.gtk.dialog), m.gtk.scrolled);

    GtkWidget* box = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(m.gtk.scrolled), box);

    GtkWidget* grid_wifi = add_grid(APPSTRING(IDS_BOX_WIFI_CONFIG), box);
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_SSID,        APP_WINDOW_LONG_WIDTH,  0, 0, m.device.m.wifi_ssid,         sizeof (m.device.m.wifi_ssid)));
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_PASSWORD,    APP_WINDOW_LONG_WIDTH,  2, 0, m.device.m.wifi_password,     sizeof (m.device.m.wifi_password)));
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_PASSWORD,    APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.m.wifi_channel,      sizeof (m.device.m.wifi_channel)));

    GtkWidget* grid_mqtt = add_grid(APPSTRING(IDS_BOX_MQTT_CONFIG), box);
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_BROKER,      APP_WINDOW_LONG_WIDTH,  0, 0, m.device.m.mqtt_broker,       sizeof (m.device.m.mqtt_broker)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_ENABLE,      APP_WINDOW_SHORT_WIDTH, 2, 0, m.device.m.mqtt_enable,       sizeof (m.device.m.mqtt_enable)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_USERNAME,    APP_WINDOW_LONG_WIDTH,  0, 1, m.device.m.mqtt_username,     sizeof (m.device.m.mqtt_username)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_PASSWORD,    APP_WINDOW_LONG_WIDTH,  2, 1, m.device.m.mqtt_password,     sizeof (m.device.m.mqtt_password)));

    GtkWidget* grid_oled = add_grid(APPSTRING(IDS_BOX_DISPLAY_CONFIG), box);
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_TIMEOUT,  APP_WINDOW_SHORT_WIDTH, 0, 0, m.device.m.display_timeout_s, sizeof (m.device.m.display_timeout_s), display_timeouts));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_CONTRAST, APP_WINDOW_SHORT_WIDTH, 2, 0, m.device.m.display_contrast,  sizeof (m.device.m.display_contrast), display_contrast));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_ROTATION, APP_WINDOW_SHORT_WIDTH, 4, 0, m.device.m.display_rotoation, sizeof (m.device.m.display_rotoation), display_rotation));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_LAYOUT,   APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.m.display_layout,    sizeof (m.device.m.display_layout), display_page));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_PARAM,    APP_WINDOW_SHORT_WIDTH, 2, 1, m.device.m.display_param,     sizeof (m.device.m.display_param)));

    GtkWidget* grid_misc = add_grid(APPSTRING(IDS_BOX_MISCELLANEOUS), box);
    m.gtk.items.push_back(add_text_field(grid_misc, IDS_LED_INTENSITY,    APP_WINDOW_SHORT_WIDTH, 0, 0, m.device.m.led_intensity,     sizeof (m.device.m.led_intensity), led_intensity));
    m.gtk.items.push_back(add_text_field(grid_misc, IDS_SENSOR_TYPE,      APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.m.sensor_type,       sizeof (m.device.m.sensor_type), sensor_types_list));

    return (m.gtk.dialog);
}

void App::handle_dialog_items(bool _setup) {
    for (DialogItem*& item : m.gtk.items) {
        if (item != nullptr) {
            if ((item->field != nullptr) && (item->widget != nullptr)) {
                if (_setup) {
                    if (item->list == nullptr) {
                        gtk_entry_set_text(GTK_ENTRY(item->widget), item->field);
                    } else {
                        App::string_combobox_setup(item->widget, item->field, item->list);
                    }
                } else {
                    gchar* item_text = nullptr;
                    if (item->list == nullptr) {
                        const gchar* item_text = gtk_entry_get_text(GTK_ENTRY(item->widget));
                    } else {
                        gchar* item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(item->widget));
                    }

                    if (item_text != nullptr) {
                        memset(item->field, 0, item->length);
                        strncpy(item->field, item_text, item->length - 1);
                        g_free(item_text);
                    }
                }
            }
        }
    }
}

void App::_on_command(GtkApplication* gtk, void* callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(callback_parameter);
    OBJ_PTR(App, cbp->get_this())->on_command(cbp);
}
void App::on_command(CallbackParameter* p) {
    int64_t item_id = (int64_t)(p->get_pointer());

    DialogItem* item = nullptr;
    for (DialogItem*& dlg_item : m.gtk.items) {
        if (dlg_item != nullptr) {
            if (dlg_item->id == item_id) {
                item = dlg_item;
                break;
            }
        }
    }
    if (item == nullptr) {
        return;
    }

    switch (item_id) {
        case IDS_QUIT: {
            gtk_main_quit();
        } break;

        case IDS_COPY: {
        } break;

        case IDS_PASTE: {
        } break;

        case IDS_DEVICE_SCAN: {
        } break;

        case IDS_VERSION: {
        } break;

        case IDS_WIFI_SSID: {
        } break;

        case IDS_WIFI_PASSWORD: {
        } break;

        case IDS_WIFI_CHANNEL: {
        } break;

        case IDS_MQTT_BROKER: {
        } break;

        case IDS_MQTT_USERNAME: {
        } break;

        case IDS_MQTT_PASSWORD: {
        } break;

        case IDS_MQTT_ENABLE: {
        } break;

        case IDS_DISPLAY_TIMEOUT: {
        } break;

        case IDS_DISPLAY_ROTATION: {
        } break;

        case IDS_DISPLAY_CONTRAST: {
        } break;

        case IDS_DISPLAY_LAYOUT: {
        } break;

        case IDS_DISPLAY_PARAM: {
        } break;

        case IDS_LED_INTENSITY: {
        } break;

        case IDS_SENSOR_TYPE: {
            int selected = gtk_combo_box_get_active(GTK_COMBO_BOX(item->widget));
            if (selected >= 0) {
                gchar* selected_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(item->widget));
                if (selected_text != nullptr) {
                    printf("Selected text: #%d <%s>\n", selected, selected_text);
                    g_free(selected_text);
                }
            }
        } break;

        case IDS_IFC_ENABLE: {
        } break;

        default: {
        } break;
    }
}

size_t App::json_get(char* json_data, const char* _key, char* _buffer, size_t _length) {
    if (_key == nullptr) {
        return (0);
    }

    char key[64]{ 0 };
    snprintf(key, sizeof (key), "\"%s\"", _key);

    char* p = strstr(json_data, key);
    if (p == nullptr) { return (0); }
    p += strlen(key);

    p = strstr(p, ":");
    if (p == nullptr) { return (0); }
    p += 1;
    
    p = strstr(p, "\"");
    if (p == nullptr) { return (0); }
    p += 1;
    
    size_t i;
    for (i = 0; (i < (_length - 1)) && (*p != '\"'); i++) {
        _buffer[i] = *p++;
    }
    _buffer[i] = '\0';

    return (i);
}

class KeyList {
    public:
        const char* key;
        char* field;
        size_t length;
};

void App::import_config(char* _json_string) {
    if (_json_string == nullptr) { return; }

    KeyList key_list[] = {
        { "version",          m.device.m.version,           sizeof (m.device.m.version)           },
        { "ssid",             m.device.m.wifi_ssid,         sizeof (m.device.m.wifi_ssid)         },
        { "password",         m.device.m.wifi_password,     sizeof (m.device.m.wifi_password)     },
        { "wifi_channel",     m.device.m.wifi_channel,      sizeof (m.device.m.wifi_channel)      },
        { "mqtt_broker",      m.device.m.mqtt_broker,       sizeof (m.device.m.mqtt_broker)       },
        { "mqtt_username",    m.device.m.mqtt_username,     sizeof (m.device.m.mqtt_username)     },
        { "mqtt_password",    m.device.m.mqtt_password,     sizeof (m.device.m.mqtt_password)     },
        { "mqtt_enable",      m.device.m.mqtt_enable,       sizeof (m.device.m.mqtt_enable)       },
        { "display_layout",   m.device.m.display_layout,    sizeof (m.device.m.display_layout)    },
        { "display_param",    m.device.m.display_param,     sizeof (m.device.m.display_param)     },
        { "display_rotation", m.device.m.display_rotoation, sizeof (m.device.m.display_rotoation) },
        { "display_timeout",  m.device.m.display_timeout_s, sizeof (m.device.m.display_timeout_s) },
        { "display_contrast", m.device.m.display_contrast,  sizeof (m.device.m.display_contrast)  },
        { "sensor_type",      m.device.m.sensor_type,       sizeof (m.device.m.sensor_type)       },
        { "led_intensity",    m.device.m.led_intensity,     sizeof (m.device.m.led_intensity)     },
    };

    char parameter[64]{ 0 };
    size_t len;
    for (int i = 0; i < SIZEOFARRAY(key_list); i++) {
        len = App::json_get(_json_string, key_list[i].key, parameter, sizeof (parameter));
        if (len > 0) {
            strncpy(key_list[i].field, parameter, key_list[i].length - 1);
        }
    }
}
