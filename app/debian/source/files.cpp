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

bool Files::is_existing(const char* path) {
    struct stat st{ 0 };
    return (lstat(path, &st) != -1);
}

bool Files::subfolder(const char* path, mode_t mode) {
    bool status = true;
    struct stat st{ 0 };
    if (lstat(path, &st) != 0) {
        if ((mkdir(path, mode) != 0) && (errno != EEXIST)) {
            status = false;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        errno = ENOTDIR;
        status = false;
    }
    return (status);
}

bool Files::mkpath(const char* path, mode_t mode) {
    char copy_of_path[PATH_MAX]{ 0 };
    strncpy(copy_of_path, path, PATH_MAX - 1);

    bool status = true;
    char* pp = copy_of_path;
    char* sp = nullptr;
    while ((status == true) && ((sp = strchr(pp, '/')) != nullptr)) {
        if (sp != pp) {
            *sp = 0;
            status = subfolder(copy_of_path, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }

    if (status == true) {
        status = subfolder(copy_of_path, mode);
    }

    return (status);
}

bool Files::open_file(int& fd, const char* path, int flags) {
    if (!Files::is_existing(path)) {
        if ((flags & O_ACCMODE) == O_RDONLY) {
            fd = -1;
            return (false);
        }
        flags |= O_CREAT;
    }
    int mode = S_IRWXU | S_IRWXG | S_IRWXO;
    fd = open(path, flags, mode);
    return (fd != -1);
}

bool Files::close_file(int& fd) {
    if (fd == -1) {
        return (false);
    }
    close(fd);
    fd = -1;
    return (true);
}

bool Files::set_filepointer(int fd, int64_t position) {
    if (fd == -1) {
        return (false);
    }
    int64_t setPosition = (int64_t)lseek64(fd, (__off64_t)position, SEEK_SET);
    return (setPosition == position);
}

bool Files::move_filepointer(int fd, int64_t position_offset) {
    if (fd == -1) {
        return (false);
    }
    int64_t currentPosition = (int64_t)lseek64(fd, (__off64_t)0, SEEK_CUR);
    int64_t setPosition = (int64_t)lseek64(fd, (__off64_t)position_offset, SEEK_CUR);
    return (setPosition == (currentPosition + position_offset));
}

bool Files::move_end_of_file(int fd, int64_t* position) {
    int64_t currentPosition = (int64_t)lseek64(fd, (__off64_t)0, SEEK_END);
    if (currentPosition == -1LL) {
        return (false);
    }
    if (position != nullptr) {
        *position = currentPosition;
    }
    return (true);
}

bool Files::get_filepointer(int fd, int64_t &position) {
    if (fd == -1) {
        return (false);
    }
    position = (int64_t)lseek64(fd, (__off64_t)0, SEEK_CUR);
    return (position != -1);
}

bool Files::delete_file(const char* file_path) {
    return (unlink(file_path) == 0);
}

bool Files::get_home_dir(char* _path_buffer, size_t _length) {
    if ((_path_buffer == nullptr) || (_length < 2)) {
        return (false);
    }
    passwd* pwd = getpwuid(getuid());
    const char* homedir = pwd->pw_dir;
    strncpy(_path_buffer, homedir, _length);
    return (true);
}

bool Files::get_home_dir(std::string& _path_buffer, const char* _folder) {
    _path_buffer = getpwuid(getuid())->pw_dir;
    if (_folder != nullptr) {
        _path_buffer.append(_folder);
    }
    return (true);
}

bool Files::write_data(int fd, void* data, size_t length, size_t* written_length) {
    if ((fd == -1) || (data == nullptr) || (length == 0)) {
        return (false);
    }
    ssize_t size = write(fd, data, length);
    if (written_length != nullptr) {
        *written_length = (size_t)size;
    }
    return (size == (ssize_t)length);
}

bool Files::read_data(int fd, void* data, size_t length, size_t* read_length) {
    if ((fd == -1) || (data == nullptr) || (length == 0)) {
        return (false);
    }
    ssize_t size = read(fd, data, length);
    if (read_length != nullptr) {
        *read_length = (size_t)size;
    }
    return (size == (ssize_t)length);
}

bool Files::write_data_at(int fd, int64_t position, void* data, size_t length, size_t* written_length) {
    if ((fd == -1) || (data == nullptr) || (length == 0)) {
        return (false);
    }
    ssize_t size = pwrite64(fd, data, length, position);
    if (written_length != nullptr) {
        *written_length = (size_t)size;
    }
    return (size == (ssize_t)length);
}

bool Files::read_data_from(int fd, int64_t position, void* data, size_t length, size_t* read_length) {
    if ((fd == -1) || (data == nullptr) || (length == 0)) {
        return (false);
    }
    ssize_t size = pread(fd, data, length, position);
    if (read_length != nullptr) {
        *read_length = (size_t)size;
    }
    return (size == (ssize_t)length);
}

bool Files::flush_file_buffers(int fd) {
    if (fd < 0) return (false);
    return ((fsync(fd) < 0) ? false : true);
}
