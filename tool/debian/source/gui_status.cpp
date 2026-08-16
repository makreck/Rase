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


void App::begin_status_updates(void) {
    m.gtk.timer_id = g_timeout_add(200, App::_status_task, this);

    char message[32]{ 0 };
    snprintf(message, sizeof (message), "%zu devices found", m.device_list.size());
    
    const char* status = (strlen(m.device.id.device_serial_number) > 0) ? APPSTRING(IDS_CONNECTED) : APPSTRING(IDS_NOT_CONNECTED);

    set_status(status, m.device.tty_ifac, m.device.ip_ifac, message);
}

void App::set_status(const char* _box_0, const char* _box_1, const char* _box_2, const char* _box_3) {
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
        usleep(20000);
    }
}

gboolean App::_status_task(gpointer _user_param) {
    return (APP_PTR(_user_param)->status_task());
}
gboolean App::status_task(void) {
    for (size_t i = 0; i < SIZEOFARRAY(m.gtk.status); i++) {
        if ((m.gtk.status[i].modified) && (m.gtk.status[i].widget != nullptr)) {
            m.gtk.status[i].modified = false;
            gtk_label_set_text(GTK_LABEL(m.gtk.status[i].widget), m.gtk.status[i].message);
            gtk_widget_queue_draw(m.gtk.status[i].widget);
        }
    }
    return (TRUE);
}
