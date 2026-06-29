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

const ToolbarItems mainToolbar[] = {
    { svg_search,       (void*)IDS_DEVICE_SCAN },

};
const size_t sizeOf_mainToolbar = SIZEOFARRAY(mainToolbar);


app_err_t Application::startUI(void) {
    gtk_init(&m.argumentsCount, &m.argumentsList);
    m.gtkApp = gtk_application_new(nullptr, APP_FLAGS);
    g_signal_connect(m.gtkApp, "activate", G_CALLBACK(Application::_activate), this);
    return (AppState::ok);
}

gboolean Application::_activate(GtkApplication* gtk, void* user_data) {
    APP_PTR(user_data)->activate();
    return (false);
}
void Application::activate(void) {
    createAppWindow();
    start_update();
    gtk_main();
}

void Application::setMainWindowCallbacks(void) {
    g_signal_connect(m.gtk.win, "destroy",           G_CALLBACK(gtk_main_quit),           nullptr);
    g_signal_connect(m.gtk.win, "configure-event",   G_CALLBACK(Application::_configure), this);
    g_signal_connect(m.gtk.win, "realize",           G_CALLBACK(Application::_realize),   this);
}

gboolean Application::_configure(GtkWindow* parentWindow, GdkEvent* event, void* user_data) {
    APP_PTR(user_data)->configure(event);
    return (ok);
}
void Application::configure(GdkEvent* event) {
    if (event->type == GdkEventType::GDK_CONFIGURE) {
        GdkEventConfigure* ev = (GdkEventConfigure*)event;
        onMoveOrSize((int)ev->x, (int)ev->y, (int)ev->width, (int)ev->height);
    }
}

gboolean Application::_realize(GtkWidget* widget, void* user_data) {
    APP_PTR(user_data)->realize(widget);
    return (ok);
}
void Application::realize(GtkWidget* widget) {
}

app_err_t Application::createAppWindow(void) {
    getMainWindowPlacing();

    GdkPixbuf* icon = GtkTool::svg2image(svg_rel_humidity, 64, 64, C_WHITE);
    if (icon != nullptr) {
        gtk_window_set_icon((GtkWindow*)m.gtk.win, icon);
    }

    gtk_container_set_border_width(GTK_CONTAINER(m.gtk.win), 4);

    gtk_window_set_title(GTK_WINDOW(m.gtk.win), APP_WINDOW_NAME);
    gtk_window_set_default_size(GTK_WINDOW(m.gtk.win), m.rc.client.width, m.rc.client.height);
    gtk_window_set_position(GTK_WINDOW(m.gtk.win), GTK_WIN_POS_CENTER);
    gtk_window_move(GTK_WINDOW(m.gtk.win), m.rc.client.x, m.rc.client.y - GTK_WINDOW_TITLEBAR_HEIGHT);

    createLayout();
    setMainWindowCallbacks();

    gtk_widget_show_all(m.gtk.win);
    gtk_window_present(GTK_WINDOW(m.gtk.win));

    return (ok);
}

void Application::getMainWindowPlacing(void) {
    m.rc.client.set(64, 64, APP_WINDOW_MIN_WIDTH, APP_WINDOW_MIN_HEIGHT);
    get_config()->get_main_window_pos(m.rc.client);

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
}

void Application::onMoveOrSize(int x, int y, int width, int height) {
    m.rc.client.set(x, y, width, height);

    Config* cfg = get_config();
    cfg->set_main_window_pos(m.rc.client);
    cfg->set_main_window_divider(std::min(580, (int)gtk_paned_get_position(GTK_PANED(m.gtk.paned))));
}

bool Application::_dividerCallback(void* user_param) {
    if (user_param != nullptr) {
        return (APP_PTR(user_param)->dividerCallback());
    }
    return (false);
}
bool Application::dividerCallback(void) {
    return (true);
}

gboolean Application::_paned(GtkPaned* paned, GtkScrollType* scroll_type, gpointer user_data) {
    return (APP_PTR(user_data)->paned(paned, scroll_type));
}
bool Application::paned(GtkPaned* paned, GtkScrollType* scroll_type) {
    Config* cfg = get_config();
    if (cfg != nullptr) {
        int pos = std::min(APP_WINDOW_MIN_WIDTH + 64, (int)gtk_paned_get_position(GTK_PANED(m.gtk.paned)));
        cfg->set_main_window_divider(pos);
    }
    return (true);
}

void Application::_rowSelected(GtkListBox* self, GtkListBoxRow* row, gpointer user_data) {
    APP_PTR(user_data)->rowSelected(row);
}
void Application::rowSelected(GtkListBoxRow* row) {
}

void Application::_onCommand(GtkApplication* gtk, void* callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(callback_parameter);
    OBJ_PTR(Application, cbp->get_this())->onCommand(cbp);
}
void Application::onCommand(CallbackParameter* p) {
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
            if (m.bus != nullptr) {
                m.bus->start_scan();
            }
        } break;

        default: {
        } break;
    }
}

void Application::createLayout(void) {
    m.gtk.baseVBox = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(m.gtk.win), m.gtk.baseVBox);

    gtk_box_pack_start(GTK_BOX(m.gtk.baseVBox), createMainMenu(),    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(m.gtk.baseVBox), createMainToolbar(), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(m.gtk.baseVBox), createDisplay(),     TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(m.gtk.baseVBox), createStatusBar(),   FALSE, FALSE, 0);
}

GtkWidget* Application::createMainMenu(void) {
    m.gtk.menuBar = gtk_menu_bar_new();

    { // file menu
        GtkWidget* fileMenu = gtk_menu_new();
        GtkWidget* fileMi = gtk_menu_item_new_with_label(APPSTRING(IDS_FILE));
        gtk_menu_shell_append(GTK_MENU_SHELL(m.gtk.menuBar), fileMi);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);

        GtkWidget* quitMi = gtk_menu_item_new_with_label(APPSTRING(IDS_QUIT));
        g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(Application::_onCommand), ON_ITEM(IDS_QUIT));
        gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);
    }

    { // edit menu
        GtkWidget* editMenu = gtk_menu_new();
        GtkWidget* editMi = gtk_menu_item_new_with_label(APPSTRING(IDS_EDIT));
        gtk_menu_shell_append(GTK_MENU_SHELL(m.gtk.menuBar), editMi);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(editMi), editMenu);

        GtkWidget* copyMi = gtk_menu_item_new_with_label(APPSTRING(IDS_COPY));
        g_signal_connect(G_OBJECT(copyMi), "activate", G_CALLBACK(Application::_onCommand), ON_ITEM(IDS_COPY));
        gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), copyMi);

        GtkWidget* pasteMi = gtk_menu_item_new_with_label(APPSTRING(IDS_PASTE));
        g_signal_connect(G_OBJECT(pasteMi), "activate", G_CALLBACK(Application::_onCommand), ON_ITEM(IDS_PASTE));
        gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), pasteMi);
    }

    return (m.gtk.menuBar);
}

GtkWidget* Application::createMainToolbar(void) {
    m.gtk.toolbar = GtkTool::create_toolbar( mainToolbar, sizeOf_mainToolbar, 
                                            &app_strings_main[APPLANG][0], IDS_MAIN_COUNT, 
                                            m.toolIconSize, G_CALLBACK(Application::_onCommand), this);
    gtk_widget_set_hexpand(m.gtk.toolbar, TRUE);
    return (m.gtk.toolbar);
}

GtkWidget* Application::createDisplay(void) {
    m.gtk.paned = gtk_paned_new(GtkOrientation::GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_wide_handle(GTK_PANED(m.gtk.paned), TRUE);
    gtk_widget_set_hexpand(m.gtk.paned, TRUE);
    gtk_widget_set_vexpand(m.gtk.paned, TRUE);
    gtk_widget_set_size_request(m.gtk.paned, -1, -1);
    gtk_paned_set_position(GTK_PANED(m.gtk.paned), get_config()->get_main_window_divider());

    m.gtk.frame_rec = display_createRecorder();
    gtk_paned_pack1(GTK_PANED(m.gtk.paned), m.gtk.frame_rec, TRUE, TRUE);

    m.gtk.frame_dev = display_createDevicelist();
    gtk_paned_pack2(GTK_PANED(m.gtk.paned), m.gtk.frame_dev, TRUE, TRUE);

    gtk_widget_set_size_request(m.gtk.frame_dev, 128, -1);
    gtk_widget_set_size_request(m.gtk.frame_rec, 128, -1);

    g_signal_connect(m.gtk.paned, "notify::position", G_CALLBACK(Application::_paned), this);

    return (m.gtk.paned);
}

GtkWidget* Application::createStatusBar(void) {
    m.gtk.statusBar = gtk_label_new("Status bar.");
    gtk_label_set_xalign(GTK_LABEL(m.gtk.statusBar), 0.0f);
    gtk_widget_set_hexpand(m.gtk.statusBar, TRUE);
    gtk_widget_set_size_request(m.gtk.statusBar, -1, 28);
    return (m.gtk.statusBar);
}

GtkWidget* Application::display_createRecorder(void) {
    m.lineRecorder = new LineRecorder(Application::_linRecEventCallback, this);

    m.gtk.recorder = m.lineRecorder->get();
    gtk_widget_set_hexpand(m.gtk.recorder, TRUE);
    gtk_widget_set_vexpand(m.gtk.recorder, TRUE);

    GtkWidget* frame = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(frame), m.gtk.recorder, TRUE, TRUE, 0);
    gtk_widget_set_hexpand(frame, TRUE);
    gtk_widget_set_vexpand(frame, TRUE);

    return (frame);
}

GtkWidget* Application::display_createDevicelist(void) {
    GtkWidget* frame = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frame), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(frame, -1, -1);
    gtk_widget_set_hexpand(frame, TRUE);
    gtk_widget_set_vexpand(frame, TRUE);
    return (frame);
}

void Application::buildDeviceListWidget(void) {
    std::vector<SensorConnection*> device_list = m.bus->aquire_device_list();
    for (SensorConnection*& sc : device_list) {
        if (sc != nullptr) {
            sc->set_widget();
        }
    }

    if (m.gtk.devList != nullptr) {
        gtk_container_remove(GTK_CONTAINER(m.gtk.frame_dev), GTK_WIDGET(m.gtk.devList));
    }

    m.gtk.devList = gtk_list_box_new();
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(m.gtk.devList), true);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(m.gtk.devList), GtkSelectionMode::GTK_SELECTION_SINGLE);
    gtk_widget_set_hexpand(m.gtk.devList, TRUE);
    gtk_widget_set_vexpand(m.gtk.devList, TRUE);

    int row_index = 0;
    for (SensorConnection*& sc : device_list) {
        if (sc != nullptr) {
            gtk_list_box_insert(GTK_LIST_BOX(m.gtk.devList),
                sc->set_widget(new SensorWidget(sc, row_index + 1)), row_index++);
        }
    }

    g_signal_connect(m.gtk.devList, "row-selected", G_CALLBACK(Application::_rowSelected), this);
    gtk_container_add(GTK_CONTAINER(m.gtk.frame_dev), GTK_WIDGET(m.gtk.devList));
    gtk_widget_show_all(m.gtk.devList);

    m.bus->release_device_list();
}

void Application::set_status_text(const char* _status_text) {
    memset(m.status_text, 0, sizeof (m.status_text));
    if (_status_text != nullptr) {
        strncpy(m.status_text, _status_text, sizeof (m.status_text) - 1);
    }

    uint64_t timestamp = Times::get_tick_count64();
    if (timestamp > m.status_timestamp) {
        m.status_timestamp = timestamp + 50;
        gdk_threads_add_idle(Application::_statusUpdate, this);
    }
}

gboolean Application::_statusUpdate(gpointer object) {
    APP_PTR(object)->statusUpdate();
    return ((gboolean)0);
}
void Application::statusUpdate(void) {
    gtk_label_set_text(GTK_LABEL(m.gtk.statusBar), m.status_text);
}

gboolean Application::_busUpdate(gpointer object) {
    APP_PTR(object)->busUpdate();
    return ((gboolean)0);
}
void Application::busUpdate(void) {
    buildDeviceListWidget();
}

void* Application::_updateThreadProc(void* object) {
    APP_PTR(object)->updateThreadProc();
    return (nullptr);
}
void Application::updateThreadProc(void) {
    while (!m.fTerminate) {
        Times::delay_ms(250);
        gdk_threads_add_idle(Application::_idle_update_task, this);
    }
}

gboolean Application::_idle_update_task(gpointer object) {
    APP_PTR(object)->idle_update_task();
    return ((gboolean)false);
}
void Application::idle_update_task(void) {
    std::vector<SensorConnection*> device_list = m.bus->aquire_device_list();
    for (SensorConnection*& sc : device_list) {
        if (sc != nullptr) {
            sc->update();
        }
    }
    m.bus->release_device_list();
}

gboolean Application::_post_function_task(gpointer object) {
    StaticParameter* param = reinterpret_cast<StaticParameter*>(object);
    APP_PTR(param->get_this())->post_function_task(param->get_value(), param->get_data(), param->get_size());
    delete (param);
    return ((gboolean)0);
}
void Application::post_function_task(int64_t _value, void* _data, size_t _size) {
    switch ((SensorBusMessageCode)_value) {
        case SensorBusMessageCode::rescan_request: {
            if (m.bus != nullptr) {
                m.bus->start_scan();
            }
        } break;

        default: {
        } break;
    }
}

bool Application::_linRecEventCallback(LRFindResult* eventResult, void* user_param) {
    if ((eventResult != nullptr) && (user_param != nullptr)) {
        return (APP_PTR(user_param)->linRecEventCallback(eventResult));
    }
    return (false);
}
bool Application::linRecEventCallback(LRFindResult* eventResult) {
    if (eventResult != nullptr) {
        std::string device_serial_number;
        int slot = -1;
        if (eventResult->m.node != nullptr) {
            slot = eventResult->m.node->get_slot();
            device_serial_number = eventResult->m.node->get_device_serial_number();
        }

        if (eventResult->is_type_of(LRElementType::paper, LRElementSub::curve_point)) {

            std::vector<SensorConnection*> device_list = m.bus->aquire_device_list();
            for (SensorConnection*& sc : device_list) {
                if (sc != nullptr) {
                    if (sc->is_equal_device(device_serial_number.c_str())) {
printf("Line recorder event: dev: <%s, %s>, slot #%d\n", sc->get_pid()->device_serial_number, device_serial_number.c_str(), slot + 1); // ****
                        break;
                    }
                }
            }
            m.bus->release_device_list();

        }
    }



    return (true);
}
