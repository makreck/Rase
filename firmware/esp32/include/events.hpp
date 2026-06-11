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

#include <esp_event_base.h>

#define APP_EVENT (esp_event_base_t)"App"

enum class AppEvent:uint32_t {
    none              = 0,

    display_ready     = 100,
    display_off       = 101,

    wifi_enabled      = 200,
    wifi_connected    = 201,
    wifi_disconnected = 202,
    wifi_disabled     = 203,
    wifi_AP_not_found = 204,

    web_started       = 300,
    web_shutdown      = 301,
    web_start_server  = 302,
    web_api_event     = 303,
    web_query_event   = 304,
    web_favicon_req   = 305,
    
    leds_online       = 400,
    leds_offline      = 401,

    driver_ready      = 500,
    measuring_event   = 501,

    button_ready      = 600,
    button_event      = 601,
    button_idle       = 602,
};
