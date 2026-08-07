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


// Endpoints over HTTP

#define WEB_KEY_ROOT             "/"
#define WEB_KEY_CONFIG_ROOT      "/config"
#define WEB_KEY_UPDATE_ROOT      "/update"
#define WEB_KEY_FAVICON          "/favicon.ico"

#define WEB_KEY_API_SENSORS      "/api/sensors"
#define WEB_KEY_API_IDENTIFY     "/api/id"
#define WEB_KEY_API_CONFIG       "/api/config"
#define WEB_KEY_API_REBOOT       "/api/reboot"
#define WEB_KEY_API_UPDATE       "/api/update"

// Endpoints over TTY

#define TTY_KEY_WEBSITE          "/root"
#define TTY_KEY_WIFI_SETUP       "/connect"
#define TTY_KEY_MQTT_BROKER      "/broker"
#define TTY_KEY_PAR              "/par"
#define TTY_KEY_TEST             "/test"

#define TTY_KEY_API_SENSORS      "/api/sensors"
#define TTY_KEY_API_IDENTIFY     "/api/id"
#define TTY_KEY_API_CONFIG       "/api/config"
#define TTY_KEY_API_REBOOT       "/api/reboot"
#define TTY_KEY_API_INITIALIZE   "/api/initialize"
