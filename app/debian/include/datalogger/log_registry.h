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

#pragma once

class LogRegistryChunk {
    private:
        double timecode;
        int64_t position;

    public:
        LogRegistryChunk(void) {
            clear();
        }

        LogRegistryChunk(double time, int64_t pos) {
            timecode = time;
            position = pos;
        }

        bool operator==(double _timecode) {
            double hysteresis = TC_MILLISEC / 2.0;
            return ((timecode >= (_timecode - hysteresis)) && (timecode <= (_timecode + hysteresis)));
        }

        bool operator>(double _timecode) {
            return (timecode > _timecode);
        }

        bool operator<(double _timecode) {
            return (timecode < _timecode);
        }

        void clear(void) {
            timecode = 0.0;
            position = 0;
        }

        void set(double t, int64_t pos) {
            timecode = t;
            position = pos;
        }

        void set(const LogRegistryChunk* _source) {
            if (_source != nullptr) {
                timecode = _source->timecode;
                position = _source->position;
            }
        }

        double get_timecode(void) {
            return (timecode);
        }
        
        int64_t get_position(void) {
            return (position);
        }
};

class LogRegistry {
    private:
        struct {
            int64_t count;
            int32_t step;
            int32_t index;
            int64_t count_of_records;
            int64_t first_log_position;
            int64_t next_log_position;
            double timecode_begin;
            double timecode_end;
            union {
                int64_t flags;
                struct {
                    int64_t f_modified : 1;
                    int64_t f_reserved : 63;
                };
            };
        } header; // 64 bytes

        LogRegistryChunk chunk[LOG_CHUNK_DIR_MAX];

    public:
        LogRegistry() {
            init();
        }

        ~LogRegistry() {
        }

        void    init(void);
        int     find(double _timecode);
        int64_t get_file_position_for(double _timecode);
        int64_t add(LogFrame* _frame);
        int64_t get_count_of_records(void);
        int64_t get_position(int i);
        double  get_timecode_begin(void);
        double  get_timecode_end(void);
        bool    validate_file_position(int64_t& _file_position, bool _use_for_put = false);
        bool    update(int _fd);
        bool    is_modified(void);
};
