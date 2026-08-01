/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    ESP32-S3 Station Device Firmware
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

#define WEB_KEY_ROOT             "/"
#define WEB_KEY_SENSOR_RESPONSE  "/api/sensors"
#define WEB_KEY_ID_RESPONSE      "/api/id"
#define WEB_KEY_CONFIG_ROOT      "/config"
#define WEB_KEY_CONFIG_API       "/api/config"
#define WEB_KEY_UPDATE_ROOT      "/update"
#define WEB_KEY_UPDATE_API       "/api/update"

#define CFG_KEY_WEBSITE_RESPONSE "/root"
#define CFG_KEY_SENSOR_RESPONSE  "/api/sensors"
#define CFG_KEY_ID_RESPONSE      "/api/id"
#define CFG_KEY_WIFI_SETUP       "/connect"
#define CFG_KEY_MQTT_BROKER      "/broker"
#define CFG_KEY_CONFIG           "/config"
#define CFG_KEY_INITIALIZE       "/initialize"
#define CFG_KEY_REBOOT           "/reboot"
#define CFG_KEY_PAR              "/par"
#define CFG_KEY_TEST             "/test"
