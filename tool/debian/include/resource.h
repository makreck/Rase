/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,      Preliminary Configuration Tool
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

#if GLIB_CHECK_VERSION(2, 74, 0)
    #define APP_FLAGS               (G_APPLICATION_DEFAULT_FLAGS)
#else
    #define APP_FLAGS               (G_APPLICATION_FLAGS_NONE)
#endif

#define LANGMAX (1)

#define APP_WINDOW_NAME             "Radio Sensor Config Tool"
#define APP_WINDOW_MIN_WIDTH        (512)
#define APP_WINDOW_MIN_HEIGHT       (384)
#define APP_WINDOW_MAX_WIDTH        (8192)
#define APP_WINDOW_MAX_HEIGHT       (8192)
#define APP_WINDOW_TITLEBAR_HEIGHT  (24)

#define IDS_NO_TEXT                 ("\0")

#define IDS_MAIN_COUNT              (13)
extern const char* app_strings_main[LANGMAX][IDS_MAIN_COUNT];

#define IDS_FILE                    (0)
#define IDS_EDIT                    (1)
#define IDS_HELP                    (2)
#define IDS_QUIT                    (3)
#define IDS_COPY                    (4)
#define IDS_PASTE                   (5)
#define IDS_OK                      (6)
#define IDS_YES                     (7)
#define IDS_NO                      (8)
#define IDS_CANCEL                  (9)
#define IDS_ENABLED                 (10)
#define IDS_DISABLED                (11)
#define IDS_DEVICE_SCAN             (12)

#define SVG_STROKE_COLOR "#010203"
extern const char* svg_app;
extern const char* svg_search;

class AppString {
    public:
        static const char* get(int idLanguage, int64_t idString);
};

#define APPLANG (0) 
#define APPSTRING(x) (AppString::get(APPLANG, (x)))

