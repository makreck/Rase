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

#include "app.hpp"

// #define DISPLAY_STATE

void ChannelHistory::clear(void) {
    memset(data, 0, sizeof(data));
    count     = 0;
    
    step      = 0;
    reload    = 1;
    index     = 0;

    sum       = 0.0;
    sum_count = 0;
}

void ChannelHistory::set_range(float _bottom, float _top) {
    bottom = _bottom;
    top = _top;
}

char* ChannelHistory::to_string(void) {
    size_t length = 1;
    const char *fmt_str;

    fmt_str = "%.1f";
    for (int32_t i = 0; i < index; i++) {
        char format[32]{0};
        length += snprintf(format, sizeof(format) - 1, fmt_str, data[i]);
        fmt_str = ", %.1f";
    }

    char* history_list = (char *)malloc(length + 8);
    memset(history_list, 0, length + 8);

    fmt_str = "%.1f";
    int offset = 0;
    for (int32_t i = 0; i < index; i++) {
        offset += snprintf(history_list + offset, length - offset, fmt_str, data[i]);
        fmt_str = ", %.1f";
    }

    return (history_list);
}

void ChannelHistory::add_value(float value) {
    sum += value;
    sum_count += 1.0f;

    if (step++ >= reload) {
        step = 0;
    
        sum = sum / sum_count;
        sum_count = 1;

        data[index++] = sum;
        if (index >= HIST_MAX) {
            reload++;
            index = HIST_MAX / 2;
            for (int i = 0; i < index; i++) {
                data[i] = data[i + i];
            }
        }
    }
}
