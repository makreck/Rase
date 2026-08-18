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

void App::run_gui(void) {
    gtk_init(&m.argc, &m.argv);
    search_and_select();

    m.gtkApp = gtk_application_new(nullptr, APP_FLAGS);
    g_signal_connect(m.gtkApp, "activate", G_CALLBACK(App::_activate), this);

    g_application_run(G_APPLICATION(m.gtkApp), m.argc, m.argv);
}

gboolean App::_activate(GtkApplication* gtk, void* user_data) {
    APP_PTR(user_data)->activate();
    return (false);
}
void App::activate(void) {
    create_gui();
    handle_dialog_items(true);
    begin_status_updates();
    gtk_main();
}

void App::create_window(void) {
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

    gtk_container_set_border_width(GTK_CONTAINER(m.gtk.win), 0);
    gtk_window_set_title(GTK_WINDOW(m.gtk.win), APP_WINDOW_NAME);

    GdkPixbuf* icon = App::svg2image(svg_app, 64, 64, C_WHITE);
    if (icon != nullptr) {
        gtk_window_set_icon((GtkWindow*)m.gtk.win, icon);
    }
}

void App::create_gui(void) {
    create_window();
    create_layout();
    set_main_window_callbacks();
    gtk_widget_show_all(m.gtk.win);
}

void App::set_main_window_callbacks(void) {
    g_signal_connect(m.gtk.win, "destroy",         G_CALLBACK(gtk_main_quit),   nullptr);
    g_signal_connect(m.gtk.win, "configure-event", G_CALLBACK(App::_configure), this);
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
}

void App::create_layout(void) {
    m.gtk.base_v_box = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 2);
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

GtkWidget* App::create_statusbar(void) {
    m.gtk.status_box = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_size_request(m.gtk.status_box, -1, APP_WINDOW_STATUSBAR_HEIGHT);
    gtk_container_set_border_width(GTK_CONTAINER(m.gtk.status_box), 4);

    m.gtk.status_grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(m.gtk.status_grid), FALSE);
    gtk_grid_set_column_homogeneous(GTK_GRID(m.gtk.status_grid), FALSE);
    gtk_grid_set_column_spacing(GTK_GRID(m.gtk.status_grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(m.gtk.status_grid), 4);
    gtk_box_pack_start(GTK_BOX(m.gtk.status_box), m.gtk.status_grid, true, true, 0);

    int width = m.rc.client.width / 4 - 16;
    for (int i = 0; i < SIZEOFARRAY(m.gtk.status); i++) {
        m.gtk.status[i].widget = gtk_label_new("---");
        gtk_label_set_xalign(GTK_LABEL(m.gtk.status[i].widget), 0.0f);
        gtk_widget_set_size_request(m.gtk.status[i].widget, width, -1);
        gtk_grid_attach(GTK_GRID(m.gtk.status_grid), m.gtk.status[i].widget, i, 0, 1, 1);
    }

    return (m.gtk.status_box);
}

// void* App::_interval_thread(void* _object) {
//     APP_PTR(_object)->interval_thread();
//     return (nullptr);
// }
// void App::interval_thread(void) {
//     while (true) {
//         usleep(250000);
//         if (m.update_request) {
//             m.update_request = false;

//         }
//     }
// }

void App::search_and_select(void) {
    if (m.scan_thread != 0) {
        pthread_join(m.scan_thread, nullptr);
        m.scan_thread = 0;
    }

    m.device.clear();
    memset(m.ifac, 0, sizeof (m.ifac));
    delete_device_list();
    handle_dialog_items(true);

    set_status(APPSTRING(IDS_NOT_CONNECTED), "Scanning...", "---", "---");
    pthread_create(&m.scan_thread, nullptr, App::_scan_thread, this);
}

void* App::_scan_thread(void* _object) {
    APP_PTR(_object)->scan_thread();
    return (nullptr);
}
void App::scan_thread(void) {
    m.ip_device.start_scan(&m.device_list, &m.device_list_mutex);
    EspTool::find_tty_devices(&m.device_list, &m.device_list_mutex);
    m.ip_device.wait_for_scan();

    memset(m.ifac, 0, sizeof(m.ifac));
    m.device.clear();

    for (DevConfig*& entry : m.device_list) {
        if (entry != nullptr) {
            if (entry->tty_ifac[0] != '\0') {
                strncpy(m.ifac, entry->tty_ifac, sizeof(m.ifac));
                m.device.set(entry);
                break;
            }
        }
    }

    if (m.ifac[0] == '\0') {
        for (DevConfig*& entry : m.device_list) {
            if (entry != nullptr) {
                strncpy(m.ifac, entry->ip_ifac, sizeof(m.ifac));
                m.device.set(entry);
                break;
            }
        }
    }

    gdk_threads_add_idle(App::_idle_task, ON_ITEM(this, -2));
}

gboolean App::_idle_task(gpointer _callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(_callback_parameter);
    OBJ_PTR(App, cbp->get_this())->idle_task(cbp);
    return ((gboolean)false);
}
void App::idle_task(CallbackParameter* p) {
    int item_id = p->get_item_id();
    switch(item_id) {
        case -1: {
            for (size_t i = 0; i < SIZEOFARRAY(m.gtk.status); i++) {
                if ((m.gtk.status[i].modified) && (m.gtk.status[i].widget != nullptr)) {
                    m.gtk.status[i].modified = false;
                    gtk_label_set_text(GTK_LABEL(m.gtk.status[i].widget), m.gtk.status[i].message);
                    gtk_widget_queue_draw(m.gtk.status[i].widget);
                }
            }
        } break;

        case -2: {
            if (m.scan_thread != 0) {
                pthread_join(m.scan_thread, nullptr);
                m.scan_thread = 0;
            }
            char message[32]{ 0 };
            snprintf(message, sizeof (message), "%zu devices found", m.device_list.size());
            const char* status = (strlen(m.device.id.device_serial_number) > 0) ? APPSTRING(IDS_CONNECTED) : APPSTRING(IDS_NOT_CONNECTED);
            set_status(status, m.device.tty_ifac, m.device.ip_ifac, message);
            handle_dialog_items(true);
        } break;

        case IDS_SEARCH: {
            search_and_select();
        } break;

        case IDS_RELOAD_DATA: {
            if (EspTool::read_data(m.ifac, &m.device)) {
                handle_dialog_items(true);
            }
            m.update_request = true;
        } break;

        case IDS_PROGRAM_DEV: {
            size_t length = 0;
            char* json_string = m.device.get_config_json(TTY_KEY_API_CONFIG_PUT, &length);
            if (json_string != nullptr) {
                char* response = EspTool::transact_command(m.ifac, json_string);
                if (response != nullptr) {
                    free(response);
                }
                free(json_string);
            }
            m.update_request = true;
        } break;

        case IDS_RESET_DEVICE: {
            char* response = EspTool::transact_command(m.ifac, TTY_KEY_API_REBOOT);
            if (response != nullptr) {
                free(response);
            }
            m.update_request = true;
        } break;

        case IDS_INITIALIZE_DEVICE: {
            char* response = EspTool::transact_command(m.ifac, TTY_KEY_API_INITIALIZE);
            if (response != nullptr) {
                free(response);
            }
            m.update_request = true;
        } break;

        case IDS_FIRMWARE_UPLOAD: {
            prepare_multi_progress();
            m.ota_update_thread = EspTool::update_all_devices(m.device_list, App::_ota_status_callback, this);
        }
        break;

        case IDS_EXEC_COMMAND: {
            const char* command = (const char*)p->get_parameter();
            char* response = EspTool::transact_command(m.ifac, command);
            if (response != nullptr) {
                free(response);
            }
            m.update_request = true;
        } break;

        default: {
        } break;
    }
}

void App::prepare_multi_progress(void) {
    if (m.ota_update_thread != 0) {
        pthread_join(m.ota_update_thread, nullptr);
        m.ota_update_thread = 0;
    }

    if (m.multi_command_thread != 0) {
        pthread_join(m.multi_command_thread, nullptr);
        m.multi_command_thread = 0;
    }

    m.multi_progress.clear();
    for (DevConfig*& entry : m.device_list) {
        m.multi_progress.push_back(0.0f);
    }
}

bool App::_ota_status_callback(void* _user_param, int _id, float _progress, const char* _topic, const char* _message) {
    return (APP_PTR(_user_param)->ota_status_callback(_id, _progress, _topic, _message));
}
bool App::ota_status_callback(int _id, float _progress, const char* _topic, const char* _message) {
    if ((_id < 0) || (_id >= m.multi_progress.size()) || (m.multi_progress.size() < 1)) {
        return (false);
    }

    m.multi_progress[_id] = _progress;

    float sum = 0.0f;
    for (float& value : m.multi_progress) {
        sum += value;
    }
    bool finished = (sum >= (float)m.multi_progress.size()) ? true : false;
    sum = sum / (float)m.multi_progress.size();

    char topic[32]{ 0 };
    snprintf(topic, sizeof (topic), "%zu OTA updates", m.multi_progress.size());

    char message[32]{ 0 };
    snprintf(message, sizeof (message), "%.0f %%", std::min(100.0f, sum * 100.0f));

    set_status(nullptr, nullptr, topic, (finished) ? "Complete" : message);

    return (true);
}
