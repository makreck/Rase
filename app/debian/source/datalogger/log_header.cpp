/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
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

void LogHeader::clear(void) {
    memset(buffer, 0, sizeof(buffer));
    strncpy(data.title_string, LOG_FILE_TITEL, sizeof(data.title_string) - 1);
    data.magic_id = LOG_FILE_MAGIC;
    data.file_version = LOG_FILE_VERSION;
    data.auto_update_threshold_ms = LOG_FILE_UPDATE_MS;
    data.header_update_count = 0;
    data.modified = 1;
    data.timestamp.created = Times::get_now();
    data.timestamp.updated = data.timestamp.created;
    data.time_zone = (uint32_t)(Times::get_utc_timeshift_byte() & 0xff);
}

bool LogHeader::set(const ProductID* _product_info) {
    if (_product_info == nullptr) {
        return (false);
    }
    memcpy(&data.product_info, _product_info, sizeof (data.product_info));
    data.modified = 1;
    return (true);
}

bool LogHeader::set_header_update_threshold_ms(float _time_s) {
    float t = std::max(0.0f, std::min(60.0f, _time_s));
    uint64_t t_ms = (uint64_t)(t * 1000.0f);
    data.modified |= (data.auto_update_threshold_ms == t_ms);
    data.auto_update_threshold_ms = t_ms;
    return (t == _time_s);    
}

uint64_t LogHeader::get_auto_update_threshold_ms(void) {
    return (data.auto_update_threshold_ms);
}

void LogHeader::set_modified(bool set_bit) {
    data.modified = (set_bit) ? 1 : 0;
}

void LogHeader::clear_modified(void) {
    data.modified = 0;
}

bool LogHeader::is_modified(void) {
    return (data.modified == 1);
}

bool LogHeader::register_update(void) {
    double timestamp = Times::get_now();
    if (data.timestamp.updated > timestamp) {
        return (false);
    }

    data.timestamp.updated = timestamp;
    data.header_update_count++;
    data.modified = 1;

    return (true);
}

double LogHeader::get_creation_timestamp(void) {
    return (data.timestamp.created);
}

double LogHeader::get_update_timestamp(void) {
    return (data.timestamp.updated);
}

double LogHeader::get_utc_offset_timecode(void) {
    return (Times::get_timecode_offset_from_utc_shift_byte(data.time_zone & 0xff));
}

bool LogHeader::must_update(uint64_t& update_timestamp) {
    bool must_update = false;

    uint64_t timestamp = Times::get_tick_count64();
    if (update_timestamp == 0) {
        update_timestamp = timestamp;
        must_update = true;
    }

    int64_t offset = data.auto_update_threshold_ms;
    int64_t delta = (int64_t)update_timestamp - (int64_t)timestamp;
    if ((offset != 0) && (delta < 0)) {
        update_timestamp += offset;
        if ((data.modified == 1)) {
            must_update = true;
        }
    }

    if (data.header_update_count < 2) {
        must_update = true;
    }

    return (must_update);    
}

ProductID* LogHeader::get_product_id(void) {
    return (&data.product_info);
}
