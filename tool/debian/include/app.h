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

#pragma once

#define TOOLBAR_BUTTON_COUNT_MAX (64)

class ToolbarItems {
    public:
        const char* svg;
        const void* text_id;
};

#define MENU_LEVEL_MAX (8)
class MenuTree {
    public:
        int level;
        int id;
};

class StatusItem {
    public:
        GtkWidget* widget = nullptr;
        char       message[64]{ 0 };
        bool       modified = false;
};

class App {
    private:
        struct {
            int argc = 0;
            char** argv = nullptr;
            char ifac[32]{ 0 };

            GtkApplication* gtkApp = nullptr;
            struct {
                GtkWidget* win         = nullptr;
                GtkWidget* base_v_box  = nullptr;
                GtkWidget* menu_bar    = nullptr;
                GtkWidget* tool_bar    = nullptr;
                
                GtkWidget* dialog      = nullptr;
                GtkWidget* scrolled    = nullptr;
                GtkWidget* status_box  = nullptr;
                GtkWidget* status_grid = nullptr;
                
                std::vector<GtkWidget*>  menu_items;
                std::vector<DialogItem*> items;
                StatusItem               status[4];
                guint                    timer_id = 0;
            } gtk;

            struct {
                GdkRectangle client;
            } rc;

            union {
                uint32_t w = 0;
                struct {
                    uint32_t use_tty     : 1;
                    uint32_t use_ip      : 1;
                    uint32_t all_devices : 1;
                    uint32_t reserved_0  : 5;

                    uint32_t reserved_1  : 8;

                    uint32_t reserved_2  : 8;

                    uint32_t n_options   : 8;
                } b;
            } flags;

            int toolIconSize = 28;
            bool update_request = false;
            pthread_t loader_thread = 0;
            pthread_t scan_thread = 0;

            char cmd[64]{ 0 };
            IPDevice ip_device;

            DevConfig device;
            pthread_mutex_t device_list_mutex = PTHREAD_MUTEX_INITIALIZER;
            std::vector<DevConfig*> device_list;
            std::vector<float> multi_progress;
            pthread_t ota_update_thread = 0;
            pthread_t multi_command_thread = 0;
        } m;

        static void print_help(void);
        static GdkPixbuf* svg2image(const char* _svg_string, int _width, int _height, ColorRef _color);
        static GtkWidget* create_toolbar(const ToolbarItems* _item_list, size_t _item_list_size, const char** _str_list, size_t _str_list_size, int _icon_size, GCallback cb, void* _user_par);
        static GtkWidget* create_menu_bar(void* _instance, GCallback _callback, MenuTree* _menu_tree, size_t _size, std::vector<GtkWidget*>& _menu_items);
        
        static int string_combobox_setup(GtkWidget* widget, const char* selected, const char* _str_list);

        static gboolean _activate(GtkApplication* gtk, void* user_data);
        void activate(void);
        static gboolean _configure(GtkWidget* widget, GdkEvent* event, void* user_data);
        void configure(GtkWidget* widget, GdkEvent* event);
        
        static void _on_command(GtkApplication* gtk, void* callback_parameter);
        void on_command(CallbackParameter* p);
        static void _on_menu(GtkApplication* gtk, void* callback_parameter);
        void on_menu(CallbackParameter* p);
        static void _on_toolbar(GtkApplication* gtk, void* callback_parameter);
        void on_toolbar(CallbackParameter* p);

        static gboolean _idle_task(gpointer _callback_parameter);
        void idle_task(CallbackParameter* p);

        static void* _scan_thread(void* _object);
        void scan_thread(void);

        static bool _ota_status_callback(void* _user_param, int _id, float _progress, const char* _topic, const char* _message);
        bool ota_status_callback(int _id, float _progress, const char* _topic, const char* _message);
        static gboolean _status_task(gpointer _user_param);
        gboolean status_task(void);
        void begin_status_updates(void);
        void prepare_multi_progress(void);
        
        void init(int argc, char* argv[]);
        void cleanup(void);


        DialogItem* get_item(int _item_id);
        DialogItem* add_text_field(GtkWidget* _grid, int _item_id, int _width, int _x, int _y, char* _field, size_t _length, const char* _list = nullptr);
        void run_gui(void);
        void create_gui(void);
        void create_window(void);
        void create_layout(void);
        void set_main_window_callbacks(void);
        void on_move_or_size(int x, int y, int width, int height);
        void set_status(const char* _box_0 = nullptr, const char* _box_1 = nullptr, const char* _box_2 = nullptr, const char* _box_3 = nullptr);
        void handle_dialog_items(bool _setup);
        void handle_item_change(DialogItem* _item, bool _setup);
        void import_data(char* _json_string, KeyList* _key_list, size_t _size);
        void search_and_select(void);

        GtkWidget* add_grid(const char* _label, GtkWidget* _parent);
        GtkWidget* create_main_menu(void);
        GtkWidget* create_main_toolbar(void);
        GtkWidget* create_dialog(void);
        GtkWidget* create_statusbar(void);

        void app_menu_handle_str_tab(int _item_id, int _base_id, int _dlg_item_id, const char* _tab, const char* _key);
        void app_menu_enable_disable(int _dlg_item_id, const char* _key);
        void app_menu_display_messagebox(const char* _string);
        void app_menu_display_rotation(void);
        void app_menu_display_timeout(int _item_id);
        void app_menu_display_layout(int _item_id);
        void app_menu_display_contrast(int _item_id);
        void app_menu_led_intensity(int _item_id);

        void get_options(void);
        void find_interfaces(void);
        void run_command(void);
        void run_single_command(const char* cmd);
        char* load_config_json(const char* cmd);
        void handle_transaction_result(char* result);
        
        void delete_device_list(void);


    public:
        App(int argc, char* argv[]) {
            init(argc, argv);
        }

        ~App() {
            cleanup();
        }

        void run(void);
};
