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
#define APP_WINDOW_DEF_HEIGHT       (940)
#define APP_WINDOW_MAX_WIDTH        (8192)
#define APP_WINDOW_MAX_HEIGHT       (8192)
#define APP_WINDOW_TITLEBAR_HEIGHT  (24)
#define APP_WINDOW_STATUSBAR_HEIGHT (28)

#define APP_WINDOW_LABEL_WIDTH      (72)
#define APP_WINDOW_INFO_WIDTH       (230)
#define APP_WINDOW_LONG_WIDTH       (256)
#define APP_WINDOW_SHORT_WIDTH      (72)

#define IDS_NO_TEXT                 ("\0")

#define IDS_MAIN_COUNT              (124)
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
#define IDS_SEARCH                  (12)
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
#define IDS_LED_COLOR_ORDER         (27)
#define IDS_SENSOR_TYPE             (28)
#define IDS_IFC_ENABLE              (29)
#define IDS_BOX_WIFI_CONFIG         (30)
#define IDS_BOX_MQTT_CONFIG         (31)
#define IDS_BOX_DISPLAY_CONFIG      (32)
#define IDS_BOX_MISCELLANEOUS       (33)
#define IDS_BOX_ID                  (34)

#define IDS_IDENTIFICATION          (35)
#define IDS_MANUFACTURER            (36)
#define IDS_PRODUCT                 (37)
#define IDS_SERIAL_NUMBER           (38)
#define IDS_FIRMWARE_VERSION        (39)
#define IDS_FIRMWARE_DATE           (40)
#define IDS_CHIP_TYPE               (41)
#define IDS_WIFI_STATION_MAC        (42)
#define IDS_WIFI_AP_MAC             (43)
#define IDS_BLUETOOTH_MAC           (44)
#define IDS_IP_ADDRESS              (45)
#define IDS_PARTITION_LABEL         (46)
#define IDS_PARTITION_SIZE          (47)
#define IDS_FLASH_CHIP_ID           (48)
#define IDS_FLASH_CHIP_SIZE         (49)
#define IDS_SPI_RAM_SIZE            (50)
#define IDS_HEAP_SIZE               (51)
#define IDS_WIFI_RSSI               (52)
#define IDS_WIFI_TX_POWER           (53)
#define IDS_SENSOR_HEAD             (54)
#define IDS_HEAD_SERIAL_NO          (55)
#define IDS_TIME_DATE               (56)
#define IDS_DISPLAY_TYPE            (57)

#define IDS_PROGRAM_DEV             (58)
#define IDS_RESET_DEVICE            (59)
#define IDS_INITIALIZE_DEVICE       (60)
#define IDS_LOADING                 (61)
#define IDS_CONNECTED               (62)
#define IDS_NOT_CONNECTED           (63)
#define IDS_SCANNING                (64)
#define IDS_INITIALIZING            (65)
#define IDS_RELOAD_DATA             (66)
#define IDS_FIRMWARE_UPLOAD         (67)
#define IDS_EXEC_COMMAND            (68)

#define IDS_ERROR_NO_DEV_CONNECTED  (69)
#define IDS_ERROR_CFG_READ_ERROR    (70)

#define IDS_LIST_ENABLE_DISABLE     (71)
#define IDS_LIST_SENSOR_TYPES       (72)
#define IDS_LIST_DISPLAY_TIMEOUTS   (73)
#define IDS_LIST_DISPLAY_CONTRAST   (74)
#define IDS_LIST_DISPLAY_PAGE       (75)
#define IDS_LIST_DISPLAY_ROTATION   (76)
#define IDS_LIST_LED_INTENSITY      (77)
#define IDS_LIST_LED_COLOR_ORDER    (78)

#define IDS_DEVICE_MENU             (79)
#define IDS_MAIN                    (80)
#define IDS_EXIT                    (81)
#define IDS_TITLE_MAIN              (82)
#define IDS_LAYOUT                  (83)
#define IDS_CONFIG                  (84)
#define IDS_REBOOT                  (85)
#define IDS_FACTORY_RESET           (86)
#define IDS_TITLE_DISPLAY           (87)
#define IDS_ROTATE                  (88)
#define IDS_CONTRAST                (89)
#define IDS_DISPLAY_OFF             (90)
#define IDS_TITLE_CONFIG            (91)
#define IDS_DISPLAY                 (92)
#define IDS_INTENSITY               (93)
#define IDS_MQTT_CLIENT             (94)
#define IDS_CONFIG_INTERFACE        (95)
#define IDS_SENSOR_SELECT           (96)
#define IDS_TITLE_CONTRAST          (97)
#define IDS_CONTRAST_100            (98)
#define IDS_CONTRAST_80             (99)
#define IDS_CONTRAST_60             (100)
#define IDS_CONTRAST_50             (101)
#define IDS_CONTRAST_40             (102)
#define IDS_CONTRAST_30             (103)
#define IDS_CONTRAST_20             (104)
#define IDS_CONTRAST_10             (105)
#define IDS_TITLE_LAYOUT            (106)
#define IDS_LAYOUT_VALUE_PAGE       (107)
#define IDS_LAYOUT_DETAILS_PAGE     (108)
#define IDS_LAYOUT_INFO_PAGE        (109)
#define IDS_TITLE_TIMEOUT           (110)
#define IDS_DISPLAY_OFF_NEVER       (111)
#define IDS_DISPLAY_OFF_10SEC       (112)
#define IDS_DISPLAY_OFF_1MIN        (113)
#define IDS_DISPLAY_OFF_5MIN        (114)
#define IDS_DISPLAY_OFF_15MIN       (115)
#define IDS_DISPLAY_OFF_30MIN       (116)
#define IDS_TITLE_LED_INTENSITY     (117)
#define IDS_LED_INTENSITY_100       (118)
#define IDS_LED_INTENSITY_75        (119)
#define IDS_LED_INTENSITY_50        (120)
#define IDS_LED_INTENSITY_25        (121)
#define IDS_LED_INTENSITY_10        (122)
#define IDS_LED_INTENSITY_1         (123)

#define SVG_STROKE_COLOR            "#010203"

extern const char* svg_app;
extern const char* svg_search;
extern const char* svg_upload;
extern const char* svg_reset;
extern const char* svg_init;
extern const char* svg_reload;
extern const char* svg_ic;

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
