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

#define HIST_MAX                 (64)
#define HIST_DEFAULT_INTERVAL_MS (600000)

class ChannelHistory {
    public:
        float   data[HIST_MAX];

        int32_t count          = 0;
        int32_t step           = 0;
        int32_t reload         = 1;
        int32_t index          = 0;

        float   average        = 0.0f;
        float   sum            = 0.0f;
        float   sum_count      = 0.0f;

        float   bottom         = 0.0f;
        float   top            = 1.0f;

        ChannelHistory(void) {
            clear();
        }

        ~ChannelHistory() {
        }

        void  clear(void);
        void  set_range(float bottom, float top);
        void  add_value(float value);
        char* to_string(void);
        
};
