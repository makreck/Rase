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

#define LANG_ENGLISH                (0)
#define LANGMAX                     (1)

#define APP_WINDOW_NAME             "Radio Sensor Config Tool"
#define APP_WINDOW_MIN_WIDTH        (320)
#define APP_WINDOW_MIN_HEIGHT       (200)
#define APP_WINDOW_DEF_WIDTH        (754)
#define APP_WINDOW_DEF_HEIGHT       (786) // (524)
#define APP_WINDOW_MAX_WIDTH        (8192)
#define APP_WINDOW_MAX_HEIGHT       (8192)
#define APP_WINDOW_TITLEBAR_HEIGHT  (24)

#define APP_WINDOW_LABEL_WIDTH      (72)
#define APP_WINDOW_INFO_WIDTH       (230)
#define APP_WINDOW_LONG_WIDTH       (256)
#define APP_WINDOW_SHORT_WIDTH      (72)

#define IDS_NO_TEXT                 ("\0")

#define IDS_MAIN_COUNT              (51)
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
#define IDS_VERSION                 (13)
#define IDS_WIFI_SSID               (14)
#define IDS_WIFI_PASSWORD           (15)
#define IDS_WIFI_CHANNEL            (16)
#define IDS_MQTT_BROKER             (17)
#define IDS_MQTT_USERNAME           (18)
#define IDS_MQTT_PASSWORD           (19)
#define IDS_MQTT_ENABLE             (20)
#define IDS_DISPLAY_TIMEOUT         (21)
#define IDS_DISPLAY_ROTATION        (22)
#define IDS_DISPLAY_CONTRAST        (23)
#define IDS_DISPLAY_LAYOUT          (24)
#define IDS_DISPLAY_PARAM           (25)
#define IDS_LED_INTENSITY           (26)
#define IDS_SENSOR_TYPE             (27)
#define IDS_IFC_ENABLE              (28)
#define IDS_BOX_WIFI_CONFIG         (29)
#define IDS_BOX_MQTT_CONFIG         (30)
#define IDS_BOX_DISPLAY_CONFIG      (31)
#define IDS_BOX_MISCELLANEOUS       (32)
#define IDS_BOX_ID                  (33)

#define IDS_IDENTIFICATION          (34)
#define IDS_MANUFACTURER            (35)
#define IDS_PRODUCT                 (36)
#define IDS_SERIAL_NUMBER           (37)
#define IDS_FIRMWARE_VERSION        (38)
#define IDS_FIRMWARE_DATE           (39)
#define IDS_CHIP_TYPE               (40)
#define IDS_WIFI_STATION_MAC        (41)
#define IDS_WIFI_AP_MAC             (42)
#define IDS_BLUETOOTH_MAC           (43)
#define IDS_IP_ADDRESS              (44)
#define IDS_WIFI_RSSI               (45)
#define IDS_WIFI_TX_POWER           (46)
#define IDS_USED_SENSOR             (47)

#define IDS_WRITE_DATA              (48)
#define IDS_RESET_DEVICE            (49)
#define IDS_INITIALIZE_DEVICE       (50)

#define SVG_STROKE_COLOR "#010203"
extern const char* svg_app;
extern const char* svg_search;
extern const char* svg_upload;
extern const char* svg_reset;
extern const char* svg_init;

class AppString {
    public:
        static inline int language = LANG_ENGLISH;

        static const char* get(int _id_language, int64_t _id_string);
        static const int   get_language(void) { return (language); }
        static const void  set_language(int _language) { if ((_language >= 0) && (_language < LANGMAX)) { language = _language; } } 
};

#define APPLANG (0) 
#define APPSTRING(x) (AppString::get(AppString::language, (x)))

class DialogItem {
    public:
        int         id;
        GtkWidget*  label;
        GtkWidget*  widget;
        char*       field;
        size_t      length;
        char*       list;

        DialogItem() {
            id     = -1;
            label  = nullptr;
            widget = nullptr;
            field  = nullptr;
            length = 0;
            list   = nullptr;
        }

        DialogItem(int _id, char* _field, size_t _length = 0, const char* _list = nullptr) {
            id     = _id;
            label  = nullptr;
            widget = nullptr;
            field  = _field;
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
                field  = _source->field;
                length = _source->length;
                set_list(_source->list);
            } else {
                id     = -1;
                label  = nullptr;
                widget = nullptr;
                field  = nullptr;
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
