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

LogHeader* LogFile::get_header(void) { 
    return (header);
}

LogInventory* LogFile::get_inventory(void) {
    return (inventory); 
}

LogRegistry* LogFile::get_registry(void) {
    return (registry);
}

bool LogFile::put(int _fd) {
    get_header()->register_update();
    if (Files::write_data_at(_fd, LOG_FILE_POS_HEADER, get_header(), sizeof(LogHeader))) {
        if (Files::write_data_at(_fd, LOG_FILE_POS_INVENTORY, get_inventory(), sizeof(LogInventory))) {
            if (Files::write_data_at(_fd, LOG_FILE_POS_REGISTRY, get_registry(), sizeof(LogRegistry))) {
                if (Files::flush_file_buffers(_fd)) {
                    get_header()->clear_modified();
                    return (true);
                }
            }
        }
    }
    return (false);
}

bool LogFile::get(int _fd) {
    Files::flush_file_buffers(_fd);
    if (Files::read_data_from(_fd, LOG_FILE_POS_HEADER, get_header(), sizeof(LogHeader))) {
        if (Files::read_data_from(_fd, LOG_FILE_POS_INVENTORY, get_inventory(), sizeof(LogInventory))) {
            if (Files::read_data_from(_fd, LOG_FILE_POS_REGISTRY, get_registry(), sizeof(LogRegistry))) {
                return (true);
            }
        }
    }
    return (false);
}

int64_t LogFile::put_frame(int _fd, LogFrame* _frame) {
    int64_t file_position = get_registry()->add(_frame);
    if (file_position != 0) {
        get_header()->set_modified();
        if (Files::write_data_at(_fd, file_position, _frame, sizeof (LogFrame))) {
            Files::flush_file_buffers(_fd);
        } else {
            file_position = 0;
        }
    }
    return (file_position);
}

bool LogFile::get_frame(int _fd, int64_t _file_position, LogFrame* _frame) {
    if (!get_registry()->validate_file_position(_file_position) || (_fd == -1) || (_frame == nullptr)) {
        return (false);
    }
    return (Files::read_data_from(_fd, _file_position, _frame, sizeof (LogFrame)));
}
