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

class App {
    private:
        struct {
            int argc = 0;
            char** argv = nullptr;
            char ifac[32]{ 0 };

            GtkApplication* gtkApp = nullptr;
            struct {
                GtkWidget* win           = nullptr;
                GtkWidget* base_v_box    = nullptr;
                GtkWidget*  menu_bar     = nullptr;
                GtkWidget*  tool_bar     = nullptr;
                
                GtkWidget*  dialog       = nullptr;
                GtkWidget*   scrolled    = nullptr;
                GtkWidget*  status_frame = nullptr;
                GtkWidget*   status_bar  = nullptr;

                std::vector<GtkWidget*>  menu_items;
                std::vector<DialogItem*> items;
            } gtk;
            struct {
                GdkRectangle client;
            } rc;

            int toolIconSize = 28;
            char status_text[256]{ 0 };

            DevConfig device;
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
        static gboolean _idle_task(gpointer _callback_parameter);
        void idle_task(CallbackParameter* p);
        
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
        void status_update(const char* _string = nullptr);
        void handle_dialog_items(bool _setup);
        void handle_item_change(DialogItem* _item, bool _setup);
        void import_data(char* _json_string, KeyList* _key_list, size_t _size);
        void on_command_scan(void);
        void on_command_program(void);
        void on_command_reset(void);
        void on_command_initialize(void);
        void on_reload_data(void);

        GtkWidget* add_grid(const char* _label, GtkWidget* _parent);
        GtkWidget* create_main_menu(void);
        GtkWidget* create_main_toolbar(void);
        GtkWidget* create_dialog(void);
        GtkWidget* create_statusbar(void);

        void run_command(char* cmd);
        char* load_config_json(char* cmd);
        void handle_transaction_result(char* result);

    public:
        App(int argc, char* argv[]) {
            init(argc, argv);
        }

        ~App() {
            cleanup();
        }

        void run(void);
};
