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

class Files {
    public:
        static bool is_existing(const char* path);

        static bool mkpath(const char* path, mode_t mode = 0);
        static bool subfolder(const char* path, mode_t mode = 0);
        static bool open_file(int& fd, const char* path, int flags = O_RDWR);
        static bool close_file(int& fd);
        static bool set_filepointer(int fd, int64_t position);
        static bool move_filepointer(int fd, int64_t position_offset);
        static bool move_end_of_file(int fd, int64_t* position);
        static bool get_filepointer(int fd, int64_t& position);
        static bool write_data(int fd, void* data, size_t length, size_t* written_length = nullptr);
        static bool read_data(int fd, void* data, size_t length, size_t* read_length = nullptr);
        static bool write_data_at(int fd, int64_t position, void* data, size_t length, size_t* written_length = nullptr);
        static bool read_data_from(int fd, int64_t position, void* data, size_t length, size_t* read_length = nullptr);
        static bool delete_file(const char* file_path);
        static bool get_home_dir(char* _path_buffer, size_t _length);
        static bool get_home_dir(std::string& _path_buffer, const char* _folder = nullptr);

};
