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

#define IDS_NO_TEXT ("\0")

enum class AppLang {
    english = 0,
    german = 1,

    count = 2,
};


#define LANGMAX ((int)AppLang::count)
#define IDS_LANG_ENGLISH                (0)
#define IDS_LANG_GERMAN                 (1)
extern const char* app_strings_lang[LANGMAX][LANGMAX];


#define IDS_MAIN_COUNT                  (13)
extern const char* app_strings_main[LANGMAX][IDS_MAIN_COUNT];

#define IDS_FILE                        (0)
#define IDS_EDIT                        (1)
#define IDS_HELP                        (2)
#define IDS_QUIT                        (3)
#define IDS_COPY                        (4)
#define IDS_PASTE                       (5)

#define IDS_OK                          (6)
#define IDS_YES                         (7)
#define IDS_NO                          (8)
#define IDS_CANCEL                      (9)
#define IDS_ENABLED                     (10)
#define IDS_DISABLED                    (11)

#define IDS_DEVICE_SCAN                 (12)

class AppString {
    public:
        static const char* get(int idLanguage, int64_t idString);
};

#define APPLANG (0) // ((int)((app->get_Config())->get_Language()))
#define APPSTRING(x) (AppString::get(APPLANG, (x)))

