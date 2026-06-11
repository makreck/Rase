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

const char* app_strings_lang[LANGMAX][LANGMAX] {
    { // English
        "English",
        "German",
    },

    { // German
        "Englisch",
        "Deutsch",
    },

};

const char* app_strings_main[LANGMAX][IDS_MAIN_COUNT] {
    
    { // English
        "File",
        "Edit",
        "Help",
        "Quit",
        "Copy",
        "Paste",

        "OK",
        "yes",
        "no",
        "cancel",
        "enabled",
        "disabled",

        "Device scan",
    },

    { // German
        "Datei",
        "Bearbeiten",
        "Hilfe",
        "Beenden",
        "Kopieren",
        "Einfügen"

        "OK",
        "ja",
        "nein",
        "Abbruch",
        "aktiviert",
        "deaktiviert",

        "Gerätesuche",
    },

};

const char* AppString::get(int idLanguage, int64_t idString) {
    if (idString >= (int64_t)65536) {
        return ((const char*)(idString));
    }
    if (((int)idLanguage < 0) || ((int)idLanguage >= LANGMAX) || (idString < 0) || (idString >= IDS_MAIN_COUNT)) {
        return (IDS_NO_TEXT);
    }
    return (app_strings_main[(int)idLanguage][(int)idString]);
}
