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

const char* svg_app    = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\"> <rect x=\"3\" y=\"3\" width=\"18\" height=\"18\" rx=\"2\" ry=\"2\"/><line x1=\"3\" y1=\"12\" x2=\"21\" y2=\"12\"/><line x1=\"12\" y1=\"3\" x2=\"12\" y2=\"21\"/></svg>\n";
const char* svg_search = "<svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"> <path fill-rule=\"evenodd\" d=\"M16.3198574,14.9056439 L21.7071068,20.2928932 L20.2928932,21.7071068 L14.9056439,16.3198574 C13.5509601,17.3729184 11.8487115,18 10,18 C5.581722,18 2,14.418278 2,10 C2,5.581722 5.581722,2 10,2 C14.418278,2 18,5.581722 18,10 C18,11.8487115 17.3729184,13.5509601 16.3198574,14.9056439 Z M10,16 C13.3137085,16 16,13.3137085 16,10 C16,6.6862915 13.3137085,4 10,4 C6.6862915,4 4,6.6862915 4,10 C4,13.3137085 6.6862915,16 10,16 Z\"/></svg>";
const char* svg_upload = "<svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"> <path fill-rule=\"evenodd\" d=\"M21,11 L21,20 C21,21.1045695 20.1045695,22 19,22 L5,22 C3.8954305,22 3,21.1045695 3,20 L3,11 L5,11 L5,20 L19,20 L19,11 L21,11 Z M13,5.41421356 L13,16 L11,16 L11,5.41421356 L7.70710678,8.70710678 L6.29289322,7.29289322 L12,1.58578644 L17.7071068,7.29289322 L16.2928932,8.70710678 L13,5.41421356 Z\"/></svg>";
const char* svg_reload = "<svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"> <path fill-rule=\"evenodd\" d=\"M12,8.02302014 L12,2.79623177 L22.5185923,12 L12,21.2037682 L12,15.8718462 C8.69020572,15.5189215 5.9539313,16.4602823 3.70710678,18.7071068 L2,20.4142136 L2,18 C2,11.7344566 5.45133717,8.33140969 12,8.02302014 Z M14,7.20376823 L14,10 L13,10 C7.79936011,10 4.94953337,11.8510227 4.20125412,15.7165365 C6.74442804,14.0145027 9.74981728,13.4445091 13.164399,14.0136061 L14,14.1528729 L14,16.7962318 L19.4814077,12 L14,7.20376823 Z\"/> </svg>";
const char* svg_reset  = "<svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"> <path fill-rule=\"evenodd\" d=\"M17.8069373,7 C16.4464601,5.07869636 14.3936238,4 12,4 C7.581722,4 4,7.581722 4,12 L2,12 C2,6.4771525 6.4771525,2 12,2 C14.8042336,2 17.274893,3.18251178 19,5.27034886 L19,2 L21,2 L21,9 L14,9 L14,7 L17.8069373,7 Z M6.19306266,17 C7.55353989,18.9213036 9.60637619,20 12,20 C16.418278,20 20,16.418278 20,12 L22,12 C22,17.5228475 17.5228475,22 12,22 C9.19576641,22 6.72510698,20.8174882 5,18.7296511 L5,22 L3,22 L3,15 L10,15 L10,17 L6.19306266,17 Z\"/> </svg>";
const char* svg_init   = "<svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"> <path fill-rule=\"evenodd\" d=\"M7.61130947,22.5952362 L5.92685415,18.0731458 L1.40476381,16.3886905 L3.41127478,12 L1.40476381,7.61130947 L5.92685415,5.92685415 L7.61130947,1.40476381 L12,3.41127478 L16.3886905,1.40476381 L18.0731458,5.92685415 L22.5952362,7.61130947 L20.5887252,12 L22.5952362,16.3886905 L18.0731458,18.0731458 L16.3886905,22.5952362 L12,20.5887252 L7.61130947,22.5952362 Z M12,18.3896052 L15.2649781,19.8823545 L16.5181332,16.5181332 L19.8823545,15.2649781 L18.3896052,12 L19.8823545,8.73502188 L16.5181332,7.48186681 L15.2649781,4.11764554 L12,5.61039477 L8.73502188,4.11764554 L7.48186681,7.48186681 L4.11764554,8.73502188 L5.61039477,12 L4.11764554,15.2649781 L7.48186681,16.5181332 L8.73502188,19.8823545 L12,18.3896052 Z M12,17 C9.23857625,17 7,14.7614237 7,12 C7,9.23857625 9.23857625,7 12,7 C14.7614237,7 17,9.23857625 17,12 C17,14.7614237 14.7614237,17 12,17 Z M12,15 C13.6568542,15 15,13.6568542 15,12 C15,10.3431458 13.6568542,9 12,9 C10.3431458,9 9,10.3431458 9,12 C9,13.6568542 10.3431458,15 12,15 Z\"/></svg>";
const char* svg_ic     = "<svg viewBox=\"0 0 122.88 122.88\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"> <path fill-rule=\"evenodd\" d=\"M28.7,122.88h11.03v-13.4H28.7V122.88L28.7,122.88z M22.67,19.51h74.76c2.56,0,4.66,2.09,4.66,4.66v75.01 c0,2.56-2.1,4.66-4.66,4.66l-74.76,0c-2.56,0-4.66-2.1-4.66-4.66V24.16C18.01,21.6,20.1,19.51,22.67,19.51L22.67,19.51L22.67,19.51 z M42.35,41.29h35.38c1.55,0,2.81,1.27,2.81,2.81v35.12c0,1.55-1.27,2.81-2.81,2.81H42.35c-1.55,0-2.81-1.27-2.81-2.81V44.1 C39.54,42.56,40.8,41.29,42.35,41.29L42.35,41.29z M122.88,65.62v9.16h-13.4v-9.16H122.88L122.88,65.62z M122.88,48.1v9.16l-13.4,0 V48.1L122.88,48.1L122.88,48.1L122.88,48.1z M122.88,83.15v11.03h-13.4V83.15H122.88L122.88,83.15z M122.88,28.7v11.03h-13.4V28.7 H122.88L122.88,28.7z M0,65.62v9.16h13.4v-9.16H0L0,65.62z M0,48.1v9.16l13.4,0V48.1L0,48.1L0,48.1z M0,83.15v11.03h13.4V83.15H0 L0,83.15z M0,28.7v11.03h13.4V28.7H0L0,28.7z M65.62,0h9.16v13.4h-9.16V0L65.62,0L65.62,0z M48.1,0h9.16v13.4H48.1V0L48.1,0L48.1,0 z M83.15,0h11.03v13.4H83.15V0L83.15,0L83.15,0z M28.7,0h11.03v13.4H28.7V0L28.7,0L28.7,0z M65.62,122.88h9.16v-13.4h-9.16V122.88 L65.62,122.88z M48.1,122.88h9.16v-13.4H48.1V122.88L48.1,122.88z M83.15,122.88h11.03v-13.4H83.15V122.88L83.15,122.88z\"/></svg>";

const char* app_strings_main[LANGMAX][IDS_MAIN_COUNT] {
    
    { // English
        "File",
        "Edit",
        "Help",
        "Quit",
        "Copy",
        "Paste",
        "OK",
        "yes",
        "no",
        "cancel",
        "enabled",
        "disabled",
        "Device scan",

        "Version",
        "SSID",
        "Password",
        "Channel",
        "Broker",
        "Username",
        "Password",
        "Enable",
        "Timeout",
        "Rotation",
        "Contrast",
        "Layout",
        "Parameter",
        "LED Intensity",
        "Sensor type",
        "Config interface enable",
        "Wifi configuration",
        "MQTT configuration",
        "Display configuration",
        "Miscellaneous",

        "Device identification data",
        "Identification",
        "Manufacturer",
        "Product",
        "Serial number",
        "Firmware version",
        "Firmware date",
        "Chip type",
        "Wifi station mac",
        "Wifi AP mac",
        "Bluetooth mac",
        "IP address",
        "RSSI",
        "Tx power",
        "Sensor head type",
        "Head serial no.",
        "System time",

        "Program device",
        "Reset device",
        "Initialize device",
        "loading ...",
        "Connected",
        "Not connected",
        "Scanning ports ...",
        "Initializing ...",
        "Reload data",
        "Firmware upload",

        "No connected device found!\n\nPlease connect a device first and ensure\nthat the config interface is enabled.",
        "Unable to read config data from device!\n\nPlease ensure that the config interface\nis enabled.",

        "disabled\nenabled",
        "autoscan\nNull\nSHT2x\nSHT3x\nHTU21d\nATHxx\nHDC1080\nBMx280",
        "Never\n10 s\n1 min\n5 min\n15 min\n30 min",
        "100%\n80%\n60%\n50%\n40%\n30%\n20%\n10%",
        "Value page\nDetails page\nInfo page",
        "0°\n180°",
        "100%\n75%\n50%\n25%\n10%\n1%",

        "Menu",
        "1 Main menu",
        "1 Exit",
        "Main menu",
        "2 Layout",
        "3 Config",
        "4 Reboot system",
        "5 Factory reset",
        "Display settings",
        "2 Rotate",
        "3 Contrast",
        "4 Timeout",
        "Config menu",
        "2 Display",
        "3 LED intensity",
        "4 MQTT client",
        "5 COM interface",
        "6 Sensor select",
        "Display contrast",
        "2 100%",
        "3 80%",
        "4 60%",
        "5 50%",
        "6 40%",
        "7 30%",
        "8 20%",
        "9 10%",
        "Screen layout",
        "2 Value page",
        "3 Details page",
        "4 Info page",
        "Display timeout",
        "2 Never",
        "3 After 10 sec.",
        "4 After 1 min.",
        "5 After 5 min.",
        "6 After 15 min.",
        "7 After 30 min.",        
        "LED intensity",
        "2 Max. 100%",
        "3 High 75%",
        "4 Medium 50%",
        "5 Low 25%",
        "6 Very low 10%",
        "7 Min. 1%",

    }
};


/*
#define IDS_DEVICE_MENU
#define IDS_MAIN
#define IDS_EXIT
#define IDS_TITLE_MAIN
#define IDS_LAYOUT
#define IDS_CONFIG
#define IDS_REBOOT
#define IDS_FACTORY_RESET
#define IDS_TITLE_DISPLAY
#define IDS_ROTATE
#define IDS_CONTRAST
#define IDS_DISPLAY_OFF
#define IDS_TITLE_CONFIG
#define IDS_DISPLAY
#define IDS_LED_INTENSITY
#define IDS_MQTT_CLIENT
#define IDS_CONFIG_INTERFACE
#define IDS_SENSOR_SELECT
#define IDS_TITLE_CONTRAST
#define IDS_CONTRAST_100
#define IDS_CONTRAST_80
#define IDS_CONTRAST_60
#define IDS_CONTRAST_40
#define IDS_CONTRAST_20
#define IDS_CONTRAST_10
#define IDS_CONTRAST_20
#define IDS_CONTRAST_10
#define IDS_TITLE_LAYOUT
#define IDS_LAYOUT_VALUE_PAGE
#define IDS_LAYOUT_DETAILS_PAGE
#define IDS_LAYOUT_INFO_PAGE
#define IDS_TITLE_TIMEOUT
#define IDS_DISPLAY_OFF_NEVER
#define IDS_DISPLAY_OFF_10SEC
#define IDS_DISPLAY_OFF_1MIN
#define IDS_DISPLAY_OFF_5MIN
#define IDS_DISPLAY_OFF_15MIN
#define IDS_DISPLAY_OFF_30MIN
#define IDS_TITLE_LED_INTENSITY
#define IDS_LED_INTENSITY_100
#define IDS_LED_INTENSITY_75
#define IDS_LED_INTENSITY_50
#define IDS_LED_INTENSITY_25
#define IDS_LED_INTENSITY_10
#define IDS_LED_INTENSITY_1
*/

const char* AppString::get(int _language, int64_t _id) {
    if (_id >= (int64_t)65536) {
        return ((const char*)(_id));
    }
    if (((int)_language < 0) || ((int)_language >= LANGMAX) || (_id < 0) || (_id >= IDS_MAIN_COUNT)) {
        return (IDS_NO_TEXT);
    }
    return (app_strings_main[(int)_language][(int)_id]);
}
