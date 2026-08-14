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

#include "includes.h"


void App::set_status(const char* _box_0, const char* _box_1, const char* _box_2, const char* _box_3) {
printf("set_status()\n");
    bool modified = false;

    const char* items[4] = { _box_0, _box_1, _box_2, _box_3 };
    for (int i = 0; i < 4; i++) {
        if (items[i] != nullptr) {
            if (strncmp(m.gtk.status[i].message, items[i], sizeof (m.gtk.status[i].message)) != 0) {
                strncpy(m.gtk.status[i].message, items[i], sizeof (m.gtk.status[i].message) - 1);
                m.gtk.status[i].modified = true;
                modified = true;
            }
        }
    }

    if (modified) {
printf("set_status() -> modified -> add idle task\n");
        g_idle_add(App::_idle_task, ON_ITEM(this, -1));
        usleep(25000);
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }
    }
}


bool App::_gui_status(void* _user_param, const char* _topic, const char* _message) {
    return (APP_PTR(_user_param)->gui_status(_topic, _message));
}
bool App::gui_status(const char* _topic, const char* _message) {
printf("gui_status() callback: <%s> <%s>\n", _topic, _message);
    set_status(nullptr, nullptr, _topic, _message);
    return (true);
}
