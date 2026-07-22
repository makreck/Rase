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


const ToolbarItems main_toolbar[] = {
    { svg_search, (void*)IDS_DEVICE_SCAN       },
    { nullptr,    (void*)nullptr               },
    { svg_reload, (void*)IDS_RELOAD_DATA       },
    { svg_upload, (void*)IDS_PROGRAM_DEV       },
    { nullptr,    (void*)nullptr               },
    { svg_reset,  (void*)IDS_RESET_DEVICE      },
    { svg_init,   (void*)IDS_INITIALIZE_DEVICE },
};
const size_t sizeOf_main_toolbar = SIZEOFARRAY(main_toolbar);

void App::run_gui(void) {
    gtk_init(&m.argc, &m.argv);

    const gchar* error_text = nullptr;

    if (DevConfig::find_interface(m.ifac, sizeof (m.ifac))) {
        if (m.device.read_data(m.ifac)) {
            m.gtkApp = gtk_application_new(nullptr, APP_FLAGS);
            g_signal_connect(m.gtkApp, "activate", G_CALLBACK(App::_activate), this);
            g_application_run(G_APPLICATION(m.gtkApp), m.argc, m.argv);
            return;
        } else {
            error_text = APPSTRING(IDS_ERROR_CFG_READ_ERROR);
        }
    } else {
        error_text = APPSTRING(IDS_ERROR_NO_DEV_CONNECTED);
    }

    if (error_text != nullptr) {
        GtkWidget* dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "\n%s\n", error_text);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

gboolean App::_activate(GtkApplication* gtk, void* user_data) {
    APP_PTR(user_data)->activate();
    return (false);
}
void App::activate(void) {
    create_app_window();
    handle_dialog_items(true);
    gtk_main();
}

void App::get_main_window_placing(void) {
    m.rc.client.x      = 64;
    m.rc.client.y      = 64;
    m.rc.client.width  = APP_WINDOW_DEF_WIDTH;
    m.rc.client.height = APP_WINDOW_DEF_HEIGHT;

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
    gtk_window_set_resizable(GTK_WINDOW(m.gtk.win), TRUE);
    gtk_window_set_position(GTK_WINDOW(m.gtk.win), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(m.gtk.win), 4);
    gtk_window_set_title(GTK_WINDOW(m.gtk.win), APP_WINDOW_NAME);

    GdkPixbuf* icon = App::svg2image(svg_app, 64, 64, C_WHITE);
    if (icon != nullptr) {
        gtk_window_set_icon((GtkWindow*)m.gtk.win, icon);
    }
}

void App::create_app_window(void) {
    get_main_window_placing();
    create_layout();
    set_main_window_callbacks();
    gtk_widget_show_all(m.gtk.win);
}

void App::set_main_window_callbacks(void) {
    g_signal_connect(m.gtk.win, "destroy",            G_CALLBACK(gtk_main_quit),   nullptr);
    g_signal_connect(m.gtk.win, "configure-event",    G_CALLBACK(App::_configure), this);
}

gboolean App::_configure(GtkWidget* widget, GdkEvent* event, void* user_data) {
    APP_PTR(user_data)->configure(widget, event);
    return (true);
}
void App::configure(GtkWidget* widget, GdkEvent* event) {
    if (event->type == GdkEventType::GDK_CONFIGURE) {
        GdkEventConfigure* ev = (GdkEventConfigure*)event;
        on_move_or_size((int)ev->x, (int)ev->y, (int)ev->width, (int)ev->height);

        gtk_widget_queue_resize(m.gtk.win);
        gtk_widget_queue_draw(m.gtk.win);
    }
}

void App::on_move_or_size(int x, int y, int width, int height) {
    m.rc.client.x = x;
    m.rc.client.y = y;
    m.rc.client.width = width;
    m.rc.client.height = height;
printf("%d,%d %d,%d\n", x, y, width, height); // ****
}

void App::create_layout(void) {
    m.gtk.base_v_box = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(m.gtk.win), m.gtk.base_v_box);

    gtk_box_pack_start(GTK_BOX(m.gtk.base_v_box), create_main_menu(),    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(m.gtk.base_v_box), create_main_toolbar(), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(m.gtk.base_v_box), create_dialog(),       TRUE,  TRUE,  0);
    gtk_box_pack_end(GTK_BOX(m.gtk.base_v_box),   create_statusbar(),    FALSE, FALSE, 0);
}

GdkPixbuf* App::svg2image(const char* _svg_string, int _width, int _height, ColorRef _color) {
    if (_svg_string == nullptr) {
        return (nullptr);
    }

    size_t length = strlen(_svg_string);
    char* modified_string = (char*)malloc(length + 8);
    memcpy(modified_string, _svg_string, length + 1);
    modified_string[length] = 0;

    char color_str[16]{ 0 };
    snprintf(color_str, sizeof (color_str), "#%-2.2X%-2.2X%-2.2X", RGBA_R(_color), RGBA_G(_color), RGBA_B(_color));
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

    RsvgRectangle viewport = { 0.0, 0.0, (double)_width, (double)_width, };
    GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, _width, _height);
    cairo_surface_t* surface = cairo_image_surface_create_for_data(gdk_pixbuf_get_pixels(pixbuf), CAIRO_FORMAT_ARGB32, _width, _height, gdk_pixbuf_get_rowstride(pixbuf));
    cairo_t* cr = cairo_create(surface);

    cairo_translate(cr, 1.0, 1.0);
    cairo_scale(cr, 1.0, 1.0);
    cairo_set_source_rgba(cr, CR_RED(_color), CR_GREEN(_color), CR_BLUE(_color), CR_ALPHA(_color));
    rsvg_handle_render_document(svg_handle, cr, &viewport, &error);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(svg_handle);

    
    free(modified_string);

    return (pixbuf);
}

GtkWidget* App::create_toolbar(const ToolbarItems* _item_list, size_t _item_list_size,
    const char** _stringList, size_t _str_list_size, int _icon_size, GCallback cb, void* _user_par) {

    if ((_item_list == nullptr) || (_item_list_size < 1) || (_item_list_size > TOOLBAR_BUTTON_COUNT_MAX)) {
        return (nullptr);
    }

    GtkToolbar* tool_bar = (GtkToolbar*)gtk_toolbar_new();
    gtk_toolbar_set_style(tool_bar, GtkToolbarStyle::GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(tool_bar, GtkIconSize::GTK_ICON_SIZE_LARGE_TOOLBAR);

    for (size_t i = 0; i < _item_list_size; i++) {
        GdkPixbuf* pixbuf = App::svg2image(_item_list[i].svg, _icon_size, _icon_size, C_WHITE);        
        if (pixbuf != nullptr) {
            GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
            g_object_unref(pixbuf);
            const char* name = nullptr;
            if ((uint64_t)(_item_list[i].text_id) > 255) {
                name = (const char*)_item_list[i].text_id;
            } else {
                int stringIndex = (int)(((uint64_t)_item_list[i].text_id) & 0xff);
                if ((stringIndex > 0) && (stringIndex < _str_list_size)) {
                    name = _stringList[stringIndex];
                }
            }

            GtkToolItem* item = (GtkToolItem *)gtk_tool_button_new(image, name);
            gtk_tool_item_set_tooltip_text(item, name);

            if (cb != nullptr) {
                g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(cb), new CallbackParameter(_user_par, (void*)_item_list[i].text_id));
            }

            gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), item, -1);
        } else if ((_item_list[i].svg == nullptr) && (_item_list[i].text_id == nullptr)) {
            GtkToolItem* seperator = gtk_separator_tool_item_new();
            gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), seperator, -1);
        }
    }

    return ((GtkWidget*)tool_bar);
}

GtkWidget* App::create_main_toolbar(void) {
    m.gtk.tool_bar = App::create_toolbar(main_toolbar, sizeOf_main_toolbar, 
                                        &app_strings_main[APPLANG][0], IDS_MAIN_COUNT, 
                                        m.toolIconSize, G_CALLBACK(App::_on_command), this);
    gtk_widget_set_hexpand(m.gtk.tool_bar, TRUE);
    return (m.gtk.tool_bar);
}

GtkWidget* App::create_statusbar(void) {
    m.gtk.status_bar = gtk_label_new("Status bar.");
    gtk_label_set_xalign(GTK_LABEL(m.gtk.status_bar), 0.0f);
    gtk_widget_set_hexpand(m.gtk.status_bar, TRUE);
    gtk_widget_set_size_request(m.gtk.status_bar, -1, 28);

    char string[256]{ 0 };
    snprintf(string, sizeof (string), "\"%s\"", m.ifac);
    status_update(string);

    return (m.gtk.status_bar);
}

void App::status_update(const char* _string) {
    if (_string != nullptr) {
        memset(m.status_text, 0, sizeof (m.status_text));
        strncpy(m.status_text, _string, sizeof (m.status_text) - 1);
    }
    gtk_label_set_text(GTK_LABEL(m.gtk.status_bar), m.status_text);
    gtk_widget_queue_draw(m.gtk.status_bar);
    gtk_widget_show_all(m.gtk.win);
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
    int i_alt = -1;
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
                    } else if (strstr(string, _selected) != nullptr) {
                        i_alt = n;
                    }
                }
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(_widget), string);
                n++;
            }
        }
    }

    if ((_selected != nullptr) && (i_sel == -1) && (i_alt == -1)) {
        if (strlen(_selected) > 0) {
            gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(_widget), 0, _selected);
            i_sel = 0;
            n++;
        }
    }

    if (i_sel == -1) {
        i_sel = i_alt;
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
    } else {
        item->widget = gtk_entry_new();
    }

    if (_width <= 0) {
        _width = (int)_length * 8;
    }
    gtk_widget_set_size_request(item->widget, std::min(320, _width), -1);

    if (_item_id >= 0) {
        g_signal_connect(item->widget, "changed", G_CALLBACK(App::_on_command), ON_ITEM(this, _item_id));
    } else {
        gtk_editable_set_editable(GTK_EDITABLE(item->widget), false);
    }

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
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_SSID,        APP_WINDOW_LONG_WIDTH,  0, 0, m.device.cfg.wifi_ssid,         sizeof (m.device.cfg.wifi_ssid)));
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_PASSWORD,    APP_WINDOW_LONG_WIDTH,  2, 0, m.device.cfg.wifi_password,     sizeof (m.device.cfg.wifi_password)));
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_CHANNEL,     APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.cfg.wifi_channel,      sizeof (m.device.cfg.wifi_channel)));

    GtkWidget* grid_mqtt = add_grid(APPSTRING(IDS_BOX_MQTT_CONFIG), box);
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_BROKER,      APP_WINDOW_LONG_WIDTH,  0, 0, m.device.cfg.mqtt_broker,       sizeof (m.device.cfg.mqtt_broker)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_ENABLE,      APP_WINDOW_SHORT_WIDTH, 2, 0, m.device.cfg.mqtt_enable,       sizeof (m.device.cfg.mqtt_enable),       APPSTRING(IDS_LIST_ENABLE_DISABLE)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_USERNAME,    APP_WINDOW_LONG_WIDTH,  0, 1, m.device.cfg.mqtt_username,     sizeof (m.device.cfg.mqtt_username)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_PASSWORD,    APP_WINDOW_LONG_WIDTH,  2, 1, m.device.cfg.mqtt_password,     sizeof (m.device.cfg.mqtt_password)));

    GtkWidget* grid_oled = add_grid(APPSTRING(IDS_BOX_DISPLAY_CONFIG), box);
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_TIMEOUT,  APP_WINDOW_SHORT_WIDTH, 0, 0, m.device.cfg.display_timeout_s, sizeof (m.device.cfg.display_timeout_s), APPSTRING(IDS_LIST_DISPLAY_TIMEOUTS)));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_CONTRAST, APP_WINDOW_SHORT_WIDTH, 2, 0, m.device.cfg.display_contrast,  sizeof (m.device.cfg.display_contrast),  APPSTRING(IDS_LIST_DISPLAY_CONTRAST)));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_ROTATION, APP_WINDOW_SHORT_WIDTH, 4, 0, m.device.cfg.display_rotoation, sizeof (m.device.cfg.display_rotoation), APPSTRING(IDS_LIST_DISPLAY_ROTATION)));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_LAYOUT,   APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.cfg.display_layout,    sizeof (m.device.cfg.display_layout),    APPSTRING(IDS_LIST_DISPLAY_PAGE)));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_PARAM,    APP_WINDOW_SHORT_WIDTH, 2, 1, m.device.cfg.display_param,     sizeof (m.device.cfg.display_param)));

    GtkWidget* grid_misc = add_grid(APPSTRING(IDS_BOX_MISCELLANEOUS), box);
    m.gtk.items.push_back(add_text_field(grid_misc, IDS_LED_INTENSITY,    APP_WINDOW_SHORT_WIDTH, 0, 0, m.device.cfg.led_intensity,     sizeof (m.device.cfg.led_intensity),     APPSTRING(IDS_LIST_LED_INTENSITY)));
    m.gtk.items.push_back(add_text_field(grid_misc, IDS_SENSOR_TYPE,      APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.cfg.sensor_type,       sizeof (m.device.cfg.sensor_type),       APPSTRING(IDS_LIST_SENSOR_TYPES)));

    DialogItem* item;
    item = get_item(IDS_WIFI_PASSWORD);
    if (item != nullptr) {
        gtk_entry_set_visibility(GTK_ENTRY(item->widget), FALSE);
    }
    item = get_item(IDS_MQTT_PASSWORD);
    if (item != nullptr) {
        gtk_entry_set_visibility(GTK_ENTRY(item->widget), FALSE);
    }

    
    GtkWidget* grid_id = add_grid(APPSTRING(IDS_BOX_ID), box);
    m.gtk.items.push_back(add_text_field(grid_id, IDS_IDENTIFICATION,   APP_WINDOW_INFO_WIDTH, 0, 0, m.device.id.identification,       sizeof (m.device.id.identification),       nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_MANUFACTURER,     APP_WINDOW_INFO_WIDTH, 2, 0, m.device.id.manufacturer,         sizeof (m.device.id.manufacturer),         nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_PRODUCT,          APP_WINDOW_INFO_WIDTH, 0, 1, m.device.id.product,              sizeof (m.device.id.product),              nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_SERIAL_NUMBER,    APP_WINDOW_INFO_WIDTH, 2, 1, m.device.id.device_serial_number, sizeof (m.device.id.device_serial_number), nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_FIRMWARE_VERSION, APP_WINDOW_INFO_WIDTH, 0, 2, m.device.id.firmware_version,     sizeof (m.device.id.firmware_version),     nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_FIRMWARE_DATE,    APP_WINDOW_INFO_WIDTH, 2, 2, m.device.id.firmware_date,        sizeof (m.device.id.firmware_date),        nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_IP_ADDRESS,       APP_WINDOW_INFO_WIDTH, 0, 3, m.device.id.ip_addr,              sizeof (m.device.id.ip_addr),              nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_CHIP_TYPE,        APP_WINDOW_INFO_WIDTH, 2, 3, m.device.id.chip_type,            sizeof (m.device.id.chip_type),            nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_SENSOR_HEAD,      APP_WINDOW_INFO_WIDTH, 0, 4, m.device.id.head,                 sizeof (m.device.id.head),                 nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_HEAD_SERIAL_NO,   APP_WINDOW_INFO_WIDTH, 2, 4, m.device.id.head_serial,          sizeof (m.device.id.head_serial),          nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_WIFI_RSSI,        APP_WINDOW_INFO_WIDTH, 0, 5, m.device.id.rssi,                 sizeof (m.device.id.rssi),                 nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_WIFI_TX_POWER,    APP_WINDOW_INFO_WIDTH, 2, 5, m.device.id.tx_power,             sizeof (m.device.id.tx_power),             nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_WIFI_STATION_MAC, APP_WINDOW_INFO_WIDTH, 0, 6, m.device.id.wifi_station_mac,     sizeof (m.device.id.wifi_station_mac),     nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_WIFI_AP_MAC,      APP_WINDOW_INFO_WIDTH, 2, 6, m.device.id.wifi_ap_mac,          sizeof (m.device.id.wifi_ap_mac),          nullptr));
    
    m.gtk.items.push_back(add_text_field(grid_id, IDS_BLUETOOTH_MAC,    APP_WINDOW_INFO_WIDTH, 0, 7, m.device.id.bluetooth_mac,        sizeof (m.device.id.bluetooth_mac),        nullptr));

    return (m.gtk.dialog);
}

DialogItem* App::get_item(int _item_id) {
    for (DialogItem*& item : m.gtk.items) {
        if (item != nullptr) {
            if (item->id == _item_id) {
                return (item);
            }
        }
    }
    return (nullptr);
}

void App::handle_item_change(DialogItem *_item, bool _setup) {
    if ((_item->field != nullptr) && (_item->widget != nullptr)) {
        if (_setup) {
            if (_item->list == nullptr) {
                gtk_entry_set_text(GTK_ENTRY(_item->widget), _item->field);
            } else {
                App::string_combobox_setup(_item->widget, _item->field, _item->list);
            }
        } else {
            if (_item->list == nullptr) {
                const gchar* item_text = gtk_entry_get_text(GTK_ENTRY(_item->widget));
                if (item_text != nullptr) {
                    memset(_item->field, 0, _item->length);
                    strncpy(_item->field, item_text, _item->length - 1);
                }
            } else {
                gchar* item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(_item->widget));
                if (item_text != nullptr) {
                    memset(_item->field, 0, _item->length);
                    strncpy(_item->field, item_text, _item->length - 1);
                    g_free(item_text);
                }
            }
        }
    }
}

void App::handle_dialog_items(bool _setup) {
    for (DialogItem*& item : m.gtk.items) {
        if (item != nullptr) {
            handle_item_change(item, _setup);
        }
    }
}

void App::_on_command(GtkApplication* gtk, void* callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(callback_parameter);
    OBJ_PTR(App, cbp->get_this())->on_command(cbp);
}
void App::on_command(CallbackParameter* p) {
    int64_t item_id = (int64_t)(p->get_pointer());

    switch (item_id) {
        case IDS_QUIT: {
            gtk_main_quit();
        } break;

        case IDS_COPY: {
        } break;

        case IDS_PASTE: {
        } break;

        case IDS_DEVICE_SCAN: {
            on_command_scan();
        } break;

        case IDS_PROGRAM_DEV: {
            on_command_program();
        } break;

        case IDS_RELOAD_DATA: {
            on_reload_data();
        } break;

        case IDS_RESET_DEVICE: {
            on_command_reset();
        } break;

        case IDS_INITIALIZE_DEVICE: {
            on_command_initialize();
        } break;

        default: {
            DialogItem* item = get_item(item_id);
            if (item != nullptr) {
                handle_item_change(item, false);
            }
        } break;
    }
}

void App::on_command_scan(void) {
    status_update(APPSTRING(IDS_SCANNING));
    if (DevConfig::find_interface(m.ifac, sizeof (m.ifac))) {
        on_reload_data();
    } else {
        status_update(APPSTRING(IDS_NOT_CONNECTED));
    }
}

void App::on_command_program(void) {
    gdk_threads_add_idle(App::_idle_task, ON_ITEM(this, IDS_PROGRAM_DEV));
}

void App::on_reload_data(void) {
    char string[256]{0};
    snprintf(string, sizeof(string), "\"%s\" --> %s", m.ifac, APPSTRING(IDS_LOADING));
    status_update(string);
    gdk_threads_add_idle(App::_idle_task, ON_ITEM(this, IDS_LOADING));
}

void App::on_command_reset(void) {
    char* response = DevConfig::transact_command(m.ifac, "/reboot");
    if (response != nullptr) {
        free(response);
    }
}

void App::on_command_initialize(void) {
    char string[256]{0};
    snprintf(string, sizeof(string), "\"%s\" --> %s", m.ifac, APPSTRING(IDS_INITIALIZING));
    status_update(string);

    char* response = DevConfig::transact_command(m.ifac, "/initialize");
    if (response != nullptr) {
        free(response);
    }
}

gboolean App::_idle_task(gpointer _callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(_callback_parameter);
    OBJ_PTR(App, cbp->get_this())->idle_task(cbp);
    return ((gboolean)false);
}
void App::idle_task(CallbackParameter* p) {
    int64_t item_id = (int64_t)(p->get_pointer());
    switch(item_id) {
        case IDS_LOADING: {
            if (m.device.read_data(m.ifac)) {
                handle_dialog_items(true);
                char string[256]{0};
                snprintf(string, sizeof(string), "\"%s\"", m.ifac);
                status_update(string);
            }
        } break;

        case IDS_PROGRAM_DEV: {
            size_t length = 0;
            char* json_string = m.device.get_config_json("/config=", &length);
            if (json_string != nullptr) {
                char* response = DevConfig::transact_command(m.ifac, json_string);
                if (response != nullptr) {
                    free(response);
                }
                free(json_string);
            }
        } break;

        case IDS_RESET_DEVICE: {
        } break;

        case IDS_INITIALIZE_DEVICE: {
        } break;
    }
}

MenuTree menu_tree[] = {
    { 1, IDS_FILE},
    {  2, IDS_QUIT},

    { 1, IDS_EDIT},
    {  2, IDS_COPY},
    {  2, IDS_PASTE},

    { 1, IDS_DEVICE_MENU},
    {  2, IDS_TITLE_MAIN},
    {  2, IDS_EXIT},
    {  2, IDS_LAYOUT},
    {   3, IDS_TITLE_LAYOUT},
    {   3, IDS_MAIN},
    {   3, IDS_LAYOUT_VALUE_PAGE},
    {   3, IDS_LAYOUT_DETAILS_PAGE},
    {   3, IDS_LAYOUT_INFO_PAGE},
    {  2, IDS_CONFIG},
    {   3, IDS_TITLE_CONFIG},
    {   3, IDS_MAIN},
    {   3, IDS_DISPLAY},
    {    4, IDS_TITLE_DISPLAY},
    {    4, IDS_MAIN},
    {    4, IDS_ROTATE},
    {    4, IDS_CONTRAST},
    {     5, IDS_TITLE_CONTRAST},
    {     5, IDS_MAIN},
    {     5, IDS_CONTRAST_100},
    {     5, IDS_CONTRAST_80},
    {     5, IDS_CONTRAST_60},
    {     5, IDS_CONTRAST_40},
    {     5, IDS_CONTRAST_20},
    {     5, IDS_CONTRAST_10},
    {     5, IDS_CONTRAST_20},
    {     5, IDS_CONTRAST_10},
    {    4, IDS_DISPLAY_OFF},
    {     5, IDS_TITLE_TIMEOUT},
    {     5, IDS_MAIN},
    {     5, IDS_DISPLAY_OFF_NEVER},
    {     5, IDS_DISPLAY_OFF_10SEC},
    {     5, IDS_DISPLAY_OFF_1MIN},
    {     5, IDS_DISPLAY_OFF_5MIN},
    {     5, IDS_DISPLAY_OFF_15MIN},
    {     5, IDS_DISPLAY_OFF_30MIN},
    {   3, IDS_INTENSITY},
    {    4, IDS_TITLE_LED_INTENSITY},
    {    4, IDS_MAIN},
    {    4, IDS_LED_INTENSITY_100},
    {    4, IDS_LED_INTENSITY_75},
    {    4, IDS_LED_INTENSITY_50},
    {    4, IDS_LED_INTENSITY_25},
    {    4, IDS_LED_INTENSITY_10},
    {    4, IDS_LED_INTENSITY_1},
    {   3, IDS_MQTT_CLIENT},
    {   3, IDS_CONFIG_INTERFACE},
    {   3, IDS_SENSOR_SELECT},
    {  2, IDS_REBOOT},
    {  2, IDS_FACTORY_RESET},

    { 0, -1 }, // End of the list
};

GtkWidget* App::create_main_menu(void) {
    std::vector<GtkWidget*> menu_items;
    m.gtk.menu_bar = App::create_menu_bar(this, G_CALLBACK(App::_on_command), menu_tree, SIZEOFARRAY(menu_tree), menu_items);

    // m.gtk.menu_bar = gtk_menu_bar_new();
    // GtkWidget* menu_stack[8]{ m.gtk.menu_bar, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    // for (int i = 0; i < (int)SIZEOFARRAY(menu_tree) - 1; i++) {
    //     const char* menu_string = APPSTRING(menu_tree[i].id);
    //     GtkWidget* item = gtk_menu_item_new_with_label(menu_string);
    //     gtk_menu_shell_append(GTK_MENU_SHELL(menu_stack[menu_tree[i].level - 1]), item);

    //     if (menu_tree[i + 1].level > menu_tree[i].level) {
    //         menu_stack[menu_tree[i].level] = gtk_menu_new();
    //         gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu_stack[menu_tree[i].level]);
    //     } else {
    //         g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(App::_on_command), ON_ITEM(this, menu_tree[i].id));
    //     }
    // }

    return (m.gtk.menu_bar);
}

GtkWidget* App::create_menu_bar(void* _instance, GCallback _callback, MenuTree* _menu_tree, size_t _size, std::vector<GtkWidget*>& _menu_items) {
    GtkWidget* menu_bar = gtk_menu_bar_new();
    _menu_items.push_back(menu_bar);

    GtkWidget* menu_stack[MENU_LEVEL_MAX]{ nullptr };
    menu_stack[0] = menu_bar;

    for (int i = 0; i < (int)_size - 1; i++) {
        const char* menu_string = APPSTRING(_menu_tree[i].id);
        GtkWidget* item = gtk_menu_item_new_with_label(menu_string);
        _menu_items.push_back(item);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu_stack[_menu_tree[i].level - 1]), item);

        if (_menu_tree[i + 1].level > _menu_tree[i].level) {
            menu_stack[_menu_tree[i].level] = gtk_menu_new();
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu_stack[_menu_tree[i].level]);
        } else {
            if (_callback != nullptr) {
                g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_callback), ON_ITEM(_instance, _menu_tree[i].id));
            }
        }
    }

    return (menu_bar);
}
