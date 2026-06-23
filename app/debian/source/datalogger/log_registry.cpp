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

void LogRegistry::init(void) {
    memset(&header, 0, sizeof (header));
    header.step               = 1;
    header.first_log_position = LOG_FILE_POS_DATA;
    header.next_log_position  = LOG_FILE_POS_DATA;

    memset(chunk, 0, sizeof (chunk));
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

int64_t LogRegistry::add(LogFrame* _frame) {
    if (_frame == nullptr) {
        return (0);
    }

    if ((header.first_log_position < LOG_FILE_POS_DATA) ||
        (header.next_log_position < LOG_FILE_POS_DATA)) {
        init();
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
        header.f_modified = 1;
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

bool LogRegistry::is_modified(void) {
    return (header.f_modified == 1);
}

bool LogRegistry::update(int _fd) {
    if (header.f_modified == 1) {
        if (Files::write_data_at(_fd, LOG_FILE_POS_REGISTRY, this, sizeof(LogRegistry))) {
            if (Files::flush_file_buffers(_fd)) {
                header.f_modified = 0;
                return (true);
            }
        }
    }
    return (false);
}

int64_t LogRegistry::get_position(int i) {
    if (i < 0) {
        return (header.first_log_position);
    }

    if (i >= (int)header.index) {
        i = std::max(0, (int)header.index - 1);
    }

    // Possibly, the file position entries are not up-to-date because the datalogger,
    // which is recording from another thread at same time, hasn't updated the registry
    // and performed a sync so far, we may be read a file position zero here. For best
    // approximation of the file position, we can try the previously written entry and
    // so on, until the start of the recording is reached.
    int64_t pos = chunk[i].get_position();
    while ((pos == 0) && (i > 0)) {
        pos = chunk[--i].get_position();
    }

    pos = std::max(header.first_log_position, std::min(header.next_log_position, pos));
    return ((int64_t)(pos / (int64_t)sizeof (LogFrame)) * (int64_t)sizeof (LogFrame));
}

int64_t LogRegistry::get_file_position_for(double _timecode) {
    return (get_position(find(_timecode)));
}

int LogRegistry::find(double _timecode) {
    if ((header.count_of_records == 0) || (_timecode < header.timecode_begin) || (header.index < 2)) {
        return (0);
    }

    int n = (int)(header.index / 2);
    int i = n;
    do {
        n = n / 2;
        if (chunk[i] > _timecode) {
            i -= n;
        } else if (chunk[i] < _timecode) {
            i += n;
        } else {
            break;
        }
    } while (n > 0);

    while ((i > 0) && (chunk[i] > _timecode)) {
        i--;
    }

    return (i);
}
