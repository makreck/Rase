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

#define MAIN_WINDOW_DEFAULT_WIDTH (1200)
#define MAIN_WINDOW_DEFAULT_HEIGHT (800)
#define MAIN_WINDOW_DEFAULT_DIVIDER (580)

#define APP_CONFIG_PATH "./" VERSION_NAME ".json"

class ConfigData : public MicroJsonObject {
    public:
        int selectedLanguage  = 0;
        int mainWindowPosX    = -1;
        int mainWindowPosY    = -1;
        int mainWindowWidth   = MAIN_WINDOW_DEFAULT_WIDTH;
        int mainWindowHeight  = MAIN_WINDOW_DEFAULT_HEIGHT;
        int mainWindowDivider = MAIN_WINDOW_DEFAULT_DIVIDER;

        const MicroJsonStruct configData[6] = {
            JSON_ITEM(ConfigData, selectedLanguage,  MicroJsonObjectType::obj_int),
            JSON_ITEM(ConfigData, mainWindowPosX,    MicroJsonObjectType::obj_int),
            JSON_ITEM(ConfigData, mainWindowPosY,    MicroJsonObjectType::obj_int),
            JSON_ITEM(ConfigData, mainWindowWidth,   MicroJsonObjectType::obj_int),
            JSON_ITEM(ConfigData, mainWindowHeight,  MicroJsonObjectType::obj_int),
            JSON_ITEM(ConfigData, mainWindowDivider, MicroJsonObjectType::obj_int),
        };
        JSON_GETTERS(ConfigData, configData);
};

class Config {
        ConfigData data;

    public:
        Config(void) {
            init();
        }

        ~Config() {
            cleanup();
        }

        void init(void);
        void cleanup(void);

        MicroJsonObject* get_object(void) { return ((MicroJsonObject*)&data); }

        void defaults(void);
        void save(void);
        void load(void);

        AppLang get_language(void);
        void set_language(AppLang selectedLanguage);
        void set_main_window_pos(RectEx& clientRect);
        void get_main_window_pos(RectEx& clientRect);
        void set_main_window_divider(int dividerPos);
        int  get_main_window_divider(void);
};
