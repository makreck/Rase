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

#if GLIB_CHECK_VERSION(2, 74, 0)
    #define APP_FLAGS               (G_APPLICATION_DEFAULT_FLAGS)
#else
    #define APP_FLAGS               (G_APPLICATION_FLAGS_NONE)
#endif

#define LANGMAX (1)

#define APP_WINDOW_NAME             "Radio Sensor Config Tool"
#define APP_WINDOW_MIN_WIDTH        (800)
#define APP_WINDOW_MIN_HEIGHT       (640)
#define APP_WINDOW_MAX_WIDTH        (8192)
#define APP_WINDOW_MAX_HEIGHT       (8192)
#define APP_WINDOW_TITLEBAR_HEIGHT  (24)
#define APP_WINDOW_LABEL_WIDTH      (72)

#define IDS_NO_TEXT                 ("\0")

#define IDS_MAIN_COUNT              (13)
extern const char* app_strings_main[LANGMAX][IDS_MAIN_COUNT];

#define IDS_FILE                    (0)
#define IDS_EDIT                    (1)
#define IDS_HELP                    (2)
#define IDS_QUIT                    (3)
#define IDS_COPY                    (4)
#define IDS_PASTE                   (5)
#define IDS_OK                      (6)
#define IDS_YES                     (7)
#define IDS_NO                      (8)
#define IDS_CANCEL                  (9)
#define IDS_ENABLED                 (10)
#define IDS_DISABLED                (11)
#define IDS_DEVICE_SCAN             (12)

#define SVG_STROKE_COLOR "#010203"
extern const char* svg_app;
extern const char* svg_search;

class AppString {
    public:
        static const char* get(int idLanguage, int64_t idString);
};

#define APPLANG (0) 
#define APPSTRING(x) (AppString::get(APPLANG, (x)))

class DialogItem {
    public:
        int         id;
        GtkWidget*  label;
        GtkWidget*  widget;
        size_t      length;
        char*       list;

        DialogItem() {
            id     = -1;
            label  = nullptr;
            widget = nullptr;
            length = 0;
            list   = nullptr;
        }

        DialogItem(int _id, size_t _length = 0, const char* _list = nullptr) {
            id     = _id;
            label  = nullptr;
            widget = nullptr;
            length = _length;
            set_list(_list);
        }

        ~DialogItem() {
            if (list != nullptr) {
                free(list);
                list = nullptr;
            }
        }

        void set(DialogItem* _source) {
            if (_source != nullptr) {
                id     = _source->id;
                label  = _source->label;
                widget = _source->widget;
                length = _source->length;
                set_list(_source->list);
            } else {
                id     = -1;
                label  = nullptr;
                widget = nullptr;
                length = 0;
                list   = nullptr;
            }
        }

        void set_list(const char* _list) {
            if (_list != nullptr) {
                size_t size = strlen(_list) + 1;
                list   = (char*)malloc(size);
                if (list != nullptr) {
                    strncpy(list, _list, size);
                }
            } else {
                list = nullptr;
            }
        }

};

#define DLG_MAX_ITEMS           (16)

#define IDC_VERSION             (0)
#define IDC_WIFI_SSID           (1)
#define IDC_WIFI_PASSWORD       (2)
#define IDC_WIFI_CHANNEL        (3)
#define IDC_MQTT_BROKER         (4)
#define IDC_MQTT_USERNAME       (5)
#define IDC_MQTT_PASSWORD       (6)
#define IDC_MQTT_ENABLE         (7)
#define IDC_DISPLAY_TIMEOUT     (8)
#define IDC_DISPLAY_ROTATION    (9)
#define IDC_DISPLAY_CONTRAST    (10)
#define IDC_DISPLAY_LAYOUT      (11)
#define IDC_DISPLAY_PARAM       (12)
#define IDC_LED_INTENSITY       (13)
#define IDC_SENSOR_TYPE         (14)
#define IDC_IFC_ENABLE          (15)

