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
        "Used sensor",
    },

};

const char* AppString::get(int _language, int64_t _id) {
    if (_id >= (int64_t)65536) {
        return ((const char*)(_id));
    }
    if (((int)_language < 0) || ((int)_language >= LANGMAX) || (_id < 0) || (_id >= IDS_MAIN_COUNT)) {
        return (IDS_NO_TEXT);
    }
    return (app_strings_main[(int)_language][(int)_id]);
}
