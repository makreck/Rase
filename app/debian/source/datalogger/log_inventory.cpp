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

void LogInventory::init(void) {
    clear();
}

void LogInventory::cleanup(void) {
}

void LogInventory::clear(void) {
    for (int i = 0; i < LOG_SLOT_MAX; i++) {
        slot[i].init();
    }
}

int LogInventory::find_slot(const char* key) {
    if (key == nullptr) {
        return (-1);
    }
    for (int i = 0; i < LOG_SLOT_MAX; i++) {
        if (!slot[i].is_key_valid()) {
            break;
        }
        if (strcmp(slot[i].get_key(), key) == 0) {
            return (i);
        }
    }
    return (-1);
}

int LogInventory::get_free_slot(void) {
    for (int i = 0; i < LOG_SLOT_MAX; i++) {
        if (!slot[i].is_key_valid()) {
            return (i);
        }
    }
    return (-1);
}

int LogInventory::add_slot(Scale* _channel_data) {
    if (_channel_data == nullptr) {
        return (-1);
    }

    int slot_index = find_slot(_channel_data->get_key());
    if (slot_index != -1) {
        return (slot_index);
    }

    slot_index = get_free_slot();
    if (slot_index != -1) {
        slot[slot_index].set(_channel_data);
    }

    return (slot_index);
}

const Scale* LogInventory::get_slot(int _index) {
    if((_index < 0) || (_index >= LOG_SLOT_MAX)) {
        return (nullptr);
    }
    if (!slot[_index].is_key_valid()) {
        return (nullptr);
    }
    return (&slot[_index]);
}

const Scale* LogInventory::get_slot(const char* _key) {
    return (get_slot(find_slot(_key)));
}

size_t LogInventory::set_userdata(const void* _userdata, size_t _length) {
    if ((_userdata != nullptr) && (_length != 0)) {
        userdata_length = (uint32_t)(std::min((size_t)LOG_USERDATA_MAX, _length));
        memcpy(userdata, _userdata, userdata_length);
    } else {
        memset(userdata, 0, sizeof (userdata));
        userdata_length = 0;
    }
    return (userdata_length);
}

size_t LogInventory::get_userdata(void* _userdata, size_t _length) {
    if ((_userdata != nullptr) && (_length != 0)) {
        size_t copy_lenght = std::min((size_t)userdata_length, (size_t)_length);
        memcpy(_userdata, userdata, copy_lenght);
        return (copy_lenght);
    }
    return (userdata_length);
}

const void* LogInventory::get_userdata(size_t* _length) {
    if (_length != nullptr) {
        *_length = userdata_length;
    }
    return (userdata);
}
