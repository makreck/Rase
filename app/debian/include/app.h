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

#if GLIB_CHECK_VERSION(2, 74, 0)
    #define APP_FLAGS (G_APPLICATION_DEFAULT_FLAGS)
#else
    #define APP_FLAGS (G_APPLICATION_FLAGS_NONE)
#endif

#define APP_WINDOW_NAME "Radio Sensor Display"

#define MAIN_WINDOW_LAYOUT "src/builder.ui.xml"

#define APP_WINDOW_MIN_WIDTH  (512)
#define APP_WINDOW_MIN_HEIGHT (384)
#define APP_WINDOW_MAX_WIDTH  (8192)
#define APP_WINDOW_MAX_HEIGHT (8192)

#define GTK_WINDOW_TITLEBAR_HEIGHT (24)

class Application {
    private:
        struct {
            int argumentsCount = 0;
            char** argumentsList = nullptr;

            Config* cfg = nullptr;
            SensorBus* bus = nullptr;
            
            bool fTerminate = false;
            int toolIconSize = 28;

            LineRecorder* lineRecorder = nullptr;

            GError* error = nullptr;
            GtkApplication* gtkApp = nullptr;
            struct {
                GtkWidget* win            = nullptr;
                GtkWidget* baseVBox       = nullptr;
                GtkWidget*  menuBar       = nullptr;
                GtkWidget*  toolbar       = nullptr;
                GtkWidget*  paned         = nullptr;
                GtkWidget*   frame_rec       = nullptr;
                GtkWidget*    recorder    = nullptr;
                GtkWidget*   frame_dev       = nullptr;
                GtkWidget*    devList     = nullptr;
                GtkWidget*  statusBar     = nullptr;
            } gtk;

            struct {
                RectEx client;
            } rc;

            pthread_t updateThread = 0;

            char status_text[1024];
            uint64_t status_timestamp = 0;
        } m;

        static gboolean _activate(GtkApplication* gtk, void* user_data);
        void activate(void);

        static gboolean _configure(GtkWindow* parentWindow, GdkEvent* event, void* user_data);
        void configure(GdkEvent* event);

        static gboolean _realize(GtkWidget* widget, void* user_data);
        void realize(GtkWidget* widget);

        static void _rowSelected(GtkListBox* self, GtkListBoxRow* row, gpointer user_data);
        void rowSelected(GtkListBoxRow* row);

        static bool _dividerCallback(void* user_param);
        bool dividerCallback(void);

        static gboolean _paned(GtkPaned* paned, GtkScrollType* scroll_type, gpointer user_data);
        bool paned(GtkPaned* paned, GtkScrollType* scroll_type);

        static gboolean _idle_update_task(gpointer object);
        void idle_update_task(void);

        static gboolean _busUpdate(gpointer object);
        void busUpdate(void);

        static gboolean _statusUpdate(gpointer object);
        void statusUpdate(void);

        static void* _updateThreadProc(void* object);
        void updateThreadProc(void);

        static void _onCommand(GtkApplication* gtk, void* callback_parameter);
        void onCommand(CallbackParameter* p);

        static bool _linRecEventCallback(LRFindResult* eventResult, void* user_param);
        bool linRecEventCallback(LRFindResult* eventResult);

        static app_err_t _sensor_bus_callback(void* _user_param, SensorBusMessageCode _message, void* _arg1, void* _arg2);
        app_err_t sensor_bus_callback(SensorBusMessageCode message, void* arg1, void* arg2);

        static gboolean _post_function_task(gpointer object);
        void post_function_task(int64_t _value, void* _data, size_t _size);

        app_err_t createAppWindow(void);
        void getMainWindowPlacing(void);
        void setMainWindowCallbacks(void);
        void createLayout(void);
        void start_update(void);
        void stop_update(void);
        GtkWidget* createMainMenu(void);
        GtkWidget* createMainToolbar(void);
        GtkWidget* createDisplay(void);
        GtkWidget* display_createDevicelist(void);
        GtkWidget* display_createRecorder(void);
        GtkWidget* createStatusBar(void);
        void buildDeviceListWidget(void);

        app_err_t init(int argc, char* argv[]);
        app_err_t cleanup(void);
        app_err_t startUI(void);

        GtkToolbar* create_toolbar(int iconSize_px, const ToolbarItems* itemList, size_t itemListSize, const char** stringList, size_t stringListSize, GCallback cb, void* parameter);

        void onMoveOrSize(int x, int y, int width, int height);
        bool select_node(const char* _key);

    public:
        Application(int argc, char* argv[]) {
            init(argc, argv);
        }

        ~Application() {
            cleanup();
        }

        Config* get_config(void) { return (m.cfg); }

        app_err_t run(void);

        void messageBox(GtkWindow* parent, gchar* message);
        void set_status_text(const char* _status_text);
};
