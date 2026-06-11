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

void Config::init(void) {
    defaults();
    load();
}

void Config::cleanup(void) {
    save();
}

void Config::defaults(void) {
    data.selectedLanguage = (int)AppLang::english;
    
    data.mainWindowPosX   = -1;
    data.mainWindowPosY   = -1;
    data.mainWindowWidth  = MAIN_WINDOW_DEFAULT_WIDTH;
    data.mainWindowHeight = MAIN_WINDOW_DEFAULT_HEIGHT;
    
    data.mainWindowDivider = -1;
}

void Config::save(void) {
    std::string s = MicroJson::encode(get_object());
    FILE* fp = fopen(APP_CONFIG_PATH, "w");
    if (fp != nullptr) {
        fputs(s.c_str(), fp);
        fclose(fp);
    }
}

void Config::load(void) {
    int fd = open(APP_CONFIG_PATH, O_RDONLY);
    if (fd != -1) {
        struct stat file_stat{ 0 };
        if (fstat(fd, &file_stat) == 0) {
            size_t length = file_stat.st_size;
            if (length > 0) {
                char* buffer = (char*)malloc(length + 1);
                memset(buffer, 0, length + 1);
                read(fd, buffer, length);
                MicroJson::parse(buffer, get_object());
                free(buffer);
            }
        }
        close(fd);
    }
}

AppLang Config::get_language(void) {
    return ((AppLang)data.selectedLanguage);
}

void Config::set_language(AppLang selectedLanguage) {
    data.selectedLanguage = (int)selectedLanguage;
}

void Config::set_main_window_pos(RectEx &clientRect) {
    data.mainWindowPosX = clientRect.x;
    data.mainWindowPosY = clientRect.y, data.mainWindowWidth = clientRect.width;
    data.mainWindowHeight = clientRect.height;
}

void Config::get_main_window_pos(RectEx &clientRect) {
    clientRect.x = data.mainWindowPosX;
    clientRect.y = data.mainWindowPosY, clientRect.width = data.mainWindowWidth;
    clientRect.height = data.mainWindowHeight;
}

void Config::set_main_window_divider(int dividerPos) {
    data.mainWindowDivider = dividerPos;
}

int Config::get_main_window_divider(void) {
    return (data.mainWindowDivider);
}
