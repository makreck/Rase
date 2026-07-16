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

class App {
    private:
        struct {
            int argc = 0;
            char** argv = nullptr;
            int fd = -1;
            char ifac[32]{ 0 };

            GtkApplication* gtkApp = nullptr;
            struct {
                GtkWidget* win           = nullptr;
                GtkWidget* baseVBox      = nullptr;
                GtkWidget*  menuBar      = nullptr;
                GtkWidget*  toolbar      = nullptr;
                
                GtkWidget*  dialog       = nullptr;
                GtkWidget*   scrolled   = nullptr;
                GtkWidget*  statusFrame = nullptr;
                GtkWidget*   statusBar  = nullptr;

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
        static GdkPixbuf* svg2image(const char* svg_string, int width_px, int height_px, ColorRef color);
        static GtkWidget* create_toolbar(const ToolbarItems* itemList, size_t itemListSize,
            const char** stringList, size_t stringListSize, int iconSize_px, GCallback cb, void* parameter);
        static int string_combobox_setup(GtkWidget* widget, const char* selected, const char* stringList);
        static size_t json_get(char* json_data, const char* _key, char* _buffer, size_t _length);

        static gboolean _activate(GtkApplication* gtk, void* user_data);
        void activate(void);
        static gboolean _configure(GtkWindow* parentWindow, GdkEvent* event, void* user_data);
        void configure(GdkEvent* event);
        static gboolean _realize(GtkWidget* widget, void* user_data);
        void realize(GtkWidget* widget);
        static void _on_command(GtkApplication* gtk, void* callback_parameter);
        void on_command(CallbackParameter* p);

        void init(int argc, char* argv[]);
        void cleanup(void);

        bool find_interface(void);
        bool open_interface(void);
        bool close_interface(void);

        int open_port(const char* ifac, speed_t baudrate);

        void run_gui(void);
        void create_app_window(void);
        void get_main_window_placing(void);
        void create_layout(void);
        void set_main_window_callbacks(void);
        void on_move_or_size(int x, int y, int width, int height);
        void status_update(const char* _string = nullptr);
        void handle_dialog_items(bool _setup);
        void import_config(char* _response);
        DialogItem* add_text_field(GtkWidget* _grid, int _item_id, int _width, int _x, int _y, char* _field, size_t _length, const char* _list = nullptr);

        GtkWidget* add_grid(const char* _label, GtkWidget* _parent);
        GtkWidget* create_main_menu(void);
        GtkWidget* create_main_toolbar(void);
        GtkWidget* create_dialog(void);
        GtkWidget* create_statusbar(void);

        void run_command(void);
        char* load_config_json(char* cmd);
        char* alloc_command(char* cmd);
        char* transact_command(const char* cmd);
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
