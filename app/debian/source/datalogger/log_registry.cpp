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

void LogRegistry::clear(void) {
    memset(&header, 0, sizeof (header));
    memset(chunk, 0, sizeof (chunk));
    header.step               = 1;
    header.first_log_position = LOG_FILE_POS_DATA;
    header.next_log_position  = LOG_FILE_POS_DATA;
}

int64_t LogRegistry::get_count_of_records(void) {
    return (header.count_of_records);
}

double LogRegistry::get_timecode_begin(void) {
    return (header.timecode_begin);
}

double LogRegistry::get_timecode_end(void) {
    return (header.timecode_end);
}

int64_t LogRegistry::get_file_position_for(double _timecode) {
    int i = find(_timecode);
    if (i < 0) {
        return (LOG_FILE_POS_DATA);
    }
    return (chunk[i].get_position());
}

int64_t LogRegistry::add(LogFrame* _frame) {
    if (_frame == nullptr) {
        return (0);
    }

    double timecode = _frame->get_timecode();

    if (header.count_of_records == 0) {
        header.timecode_begin = timecode;
    } else {
        if (timecode < header.timecode_end) {
            return (0);
        }
    }

    int64_t file_position = header.next_log_position;
    header.next_log_position += sizeof (LogFrame);

    header.timecode_end = timecode;
    header.count_of_records++;

    header.count++;
    if (header.count >= header.step) {
        header.count = 0;
        chunk[header.index++].set(timecode, file_position);
        if (header.index >= LOG_CHUNK_DIR_MAX) {
            for (header.index = 0; header.index < (LOG_CHUNK_DIR_MAX / 2); header.index++) {
                chunk[header.index] = chunk[header.index + header.index];
            }
            header.step++;
        }
    }

    return (file_position);
}

int LogRegistry::find(double _timecode) {
    if ((header.count_of_records == 0) || (_timecode < header.timecode_begin) || (header.index < 2)) {
        return (0);
    }

    int n = (int)(header.index / 2) + 1;
    int i = n;
    do {
        n = n / 2;
        if (_timecode > chunk[i].get_timecode()) {
            i -= n;
        } else if (_timecode < chunk[i].get_timecode()) {
            i += n;
        } else {
            break;
        }
    } while (n > 0);

    while ((i > 0) && (_timecode >= chunk[i].get_timecode())) {
        i--;
    }

    return (i);
}

bool LogRegistry::validate_file_position(int64_t& _file_position, bool _use_for_put) {
    bool result = true;

    int64_t begin = header.first_log_position;
    if (_file_position < begin) {
        result = false;
    }

    int64_t offset = (_file_position - begin) % sizeof (LogFrame);
    if (offset != 0) {
        _file_position -= offset;
        result = false;
    }

    return (result);
}

bool LogRegistry::update_header(int _fd) {
    if (Files::write_data_at(_fd, LOG_FILE_POS_REGISTRY, &header, sizeof (header))) {
        if (Files::flush_file_buffers(_fd)) {
            return (true);
        }
    }
    return (false);
}
