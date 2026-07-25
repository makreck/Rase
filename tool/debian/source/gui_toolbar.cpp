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

const ToolbarItems main_toolbar[] = {
    { svg_search, (void*)IDS_SEARCH            },
    { nullptr,    (void*)nullptr               },
    { svg_reload, (void*)IDS_RELOAD_DATA       },
    { svg_upload, (void*)IDS_PROGRAM_DEV       },
    { nullptr,    (void*)nullptr               },
    { svg_reset,  (void*)IDS_RESET_DEVICE      },
    { nullptr,    (void*)nullptr               },
    { svg_ic,     (void*)IDS_FIRMWARE_UPLOAD   },
    { svg_init,   (void*)IDS_INITIALIZE_DEVICE },
};
const size_t sizeOf_main_toolbar = SIZEOFARRAY(main_toolbar);

GtkWidget* App::create_main_toolbar(void) {
    m.gtk.tool_bar = App::create_toolbar(main_toolbar, sizeOf_main_toolbar, 
                                        &app_strings_main[APPLANG][0], IDS_MAIN_COUNT, 
                                        m.toolIconSize, G_CALLBACK(App::_on_toolbar), this);
    gtk_widget_set_hexpand(m.gtk.tool_bar, true);
    return (m.gtk.tool_bar);
}

GtkWidget* App::create_toolbar(const ToolbarItems* _item_list, size_t _item_list_size,
    const char** _stringList, size_t _str_list_size, int _icon_size, GCallback cb, void* _user_par) {

    if ((_item_list == nullptr) || (_item_list_size < 1) || (_item_list_size > TOOLBAR_BUTTON_COUNT_MAX)) {
        return (nullptr);
    }

    GtkToolbar* tool_bar = (GtkToolbar*)gtk_toolbar_new();
    gtk_toolbar_set_style(tool_bar, GtkToolbarStyle::GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(tool_bar, GtkIconSize::GTK_ICON_SIZE_LARGE_TOOLBAR);

    for (size_t i = 0; i < _item_list_size; i++) {
        GdkPixbuf* pixbuf = App::svg2image(_item_list[i].svg, _icon_size, _icon_size, C_WHITE);        
        if (pixbuf != nullptr) {
            GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
            g_object_unref(pixbuf);
            const char* name = nullptr;
            if ((uint64_t)(_item_list[i].text_id) > 255) {
                name = (const char*)_item_list[i].text_id;
            } else {
                int stringIndex = (int)(((uint64_t)_item_list[i].text_id) & 0xff);
                if ((stringIndex > 0) && (stringIndex < _str_list_size)) {
                    name = _stringList[stringIndex];
                }
            }

            GtkToolItem* item = (GtkToolItem *)gtk_tool_button_new(image, name);
            gtk_tool_item_set_tooltip_text(item, name);

            if (cb != nullptr) {
                g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(cb), new CallbackParameter(_user_par, (int64_t)(_item_list[i].text_id)));
            }

            gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), item, -1);
        } else if ((_item_list[i].svg == nullptr) && (_item_list[i].text_id == nullptr)) {
            GtkToolItem* seperator = gtk_separator_tool_item_new();
            gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), seperator, -1);
        }
    }

    return ((GtkWidget*)tool_bar);
}

void App::_on_toolbar(GtkApplication* gtk, void* callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(callback_parameter);
    OBJ_PTR(App, cbp->get_this())->on_toolbar(cbp);
}
void App::on_toolbar(CallbackParameter* p) {
    int item_id = p->get_item_id();
    set_status(nullptr, nullptr, APPSTRING(item_id));
    gdk_threads_add_idle(App::_idle_task, ON_ITEM(this, item_id));
}
