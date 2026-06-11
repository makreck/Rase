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
    count              = 0;
    step               = 1;
    index              = 0;
    count_of_records   = 0;
    timecode_begin     = 0.0;
    timecode_end       = 0.0;
    first_log_position = LOG_FILE_POS_DATA;
    next_log_position  = LOG_FILE_POS_DATA;
    memset(chunk, 0, sizeof (chunk));
}

int64_t LogRegistry::get_count_of_records(void) {
    return (count_of_records);
}

double LogRegistry::get_timecode_begin(void) {
    return (timecode_begin);
}

double LogRegistry::get_timecode_end(void) {
    return (timecode_end);
}

bool LogRegistry::validate_file_position(int64_t& _file_position, bool _use_for_put) {
    bool result = true;

    int64_t begin = first_log_position;
    int64_t end = (_use_for_put) ? next_log_position : (next_log_position - sizeof (LogFrame));

    if ((_file_position < begin) || (_file_position > end)) {
        result = false;
    }
    _file_position = std::max(begin, std::min(end, _file_position));

    int64_t offset = (_file_position - begin) % sizeof (LogFrame);
    if (offset != 0) {
        _file_position -= offset;
        result = false;
    }

    return (result);
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

    if (count_of_records == 0) {
        first_log_position = LOG_FILE_POS_DATA;
        next_log_position  = LOG_FILE_POS_DATA;
        timecode_begin     = timecode;
        count              = 0;
        step               = 1;
        index              = 0;
        count_of_records   = 0;
        memset(chunk, 0, sizeof (chunk));
    } else {
        if (timecode < timecode_end) {
            return (0);
        }
    }

    int64_t file_position = next_log_position;
    next_log_position += sizeof (LogFrame);

    timecode_end = timecode;
    count_of_records++;

    count++;
    if (count >= step) {
        count = 0;
        chunk[index++].set(timecode, file_position);
        if (index >= LOG_CHUNK_DIR_MAX) {
            for (index = 0; index < (LOG_CHUNK_DIR_MAX / 2); index++) {
                chunk[index] = chunk[index + index];
            }
            step++;
        }
    }

    return (file_position);
}

int LogRegistry::find(double _timecode) {
    if ((count_of_records == 0) || (_timecode < timecode_begin)) {
        return (-1);
    }

    if (index < 2) {
        return (0);
    }

    int n = (int)(index / 2) + 1;
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
