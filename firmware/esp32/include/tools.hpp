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

class Tools {
    public:
        static char* get_device_id_json(void);
        static size_t get_device_serial_number(char* buffer, size_t size);
        static AppState get_timestamp(char* buffer, size_t size, char div_char = 'T');
        static void timedate(tm& timeinfo);
        static float get_random(void);
        static uint64_t get_tickcount64(void);
        static uint32_t get_tick_seconds(void);
        static AppState get_time(char* buffer, size_t size);
        static AppState get_date(char* buffer, size_t size);
        static const char* get_build_date(void);
        static void get_iso_build_date(char* buffer, size_t size);
};
