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

MenuTree menu_tree[] = {
    { 1, IDS_FILE},
    {  2, IDS_QUIT},

    { 1, IDS_EDIT},
    {  2, IDS_COPY},
    {  2, IDS_PASTE},

    { 1, IDS_DEVICE_MENU},
    {  2, IDS_TITLE_MAIN},
    {  2, IDS_EXIT},
    {  2, IDS_LAYOUT},
    {   3, IDS_TITLE_LAYOUT},
    {   3, IDS_MAIN},
    {   3, IDS_LAYOUT_VALUE_PAGE},
    {   3, IDS_LAYOUT_DETAILS_PAGE},
    {   3, IDS_LAYOUT_INFO_PAGE},
    {  2, IDS_CONFIG},
    {   3, IDS_TITLE_CONFIG},
    {   3, IDS_MAIN},
    {   3, IDS_DISPLAY},
    {    4, IDS_TITLE_DISPLAY},
    {    4, IDS_MAIN},
    {    4, IDS_ROTATE},
    {    4, IDS_CONTRAST},
    {     5, IDS_TITLE_CONTRAST},
    {     5, IDS_MAIN},
    {     5, IDS_CONTRAST_100},
    {     5, IDS_CONTRAST_80},
    {     5, IDS_CONTRAST_60},
    {     5, IDS_CONTRAST_40},
    {     5, IDS_CONTRAST_20},
    {     5, IDS_CONTRAST_10},
    {     5, IDS_CONTRAST_20},
    {     5, IDS_CONTRAST_10},
    {    4, IDS_DISPLAY_OFF},
    {     5, IDS_TITLE_TIMEOUT},
    {     5, IDS_MAIN},
    {     5, IDS_DISPLAY_OFF_NEVER},
    {     5, IDS_DISPLAY_OFF_10SEC},
    {     5, IDS_DISPLAY_OFF_1MIN},
    {     5, IDS_DISPLAY_OFF_5MIN},
    {     5, IDS_DISPLAY_OFF_15MIN},
    {     5, IDS_DISPLAY_OFF_30MIN},
    {   3, IDS_INTENSITY},
    {    4, IDS_TITLE_LED_INTENSITY},
    {    4, IDS_MAIN},
    {    4, IDS_LED_INTENSITY_100},
    {    4, IDS_LED_INTENSITY_75},
    {    4, IDS_LED_INTENSITY_50},
    {    4, IDS_LED_INTENSITY_25},
    {    4, IDS_LED_INTENSITY_10},
    {    4, IDS_LED_INTENSITY_1},
    {   3, IDS_MQTT_CLIENT},
    {   3, IDS_CONFIG_INTERFACE},
    {   3, IDS_SENSOR_SELECT},
    {  2, IDS_REBOOT},
    {  2, IDS_FACTORY_RESET},

    { 0, -1 }, // End of the list
};

GtkWidget* App::create_main_menu(void) {
    m.gtk.menu_bar = App::create_menu_bar(this, G_CALLBACK(App::_on_menu), menu_tree, SIZEOFARRAY(menu_tree), m.gtk.menu_items);
    return (m.gtk.menu_bar);
}

GtkWidget* App::create_menu_bar(void* _instance, GCallback _callback, MenuTree* _menu_tree, size_t _size, std::vector<GtkWidget*>& _menu_items) {
    GtkWidget* menu_bar = gtk_menu_bar_new();
    _menu_items.push_back(menu_bar);

    GtkWidget* menu_stack[MENU_LEVEL_MAX]{ nullptr };
    menu_stack[0] = menu_bar;

    for (int i = 0; i < (int)_size - 1; i++) {
        const char* menu_string = APPSTRING(_menu_tree[i].id);
        GtkWidget* item = gtk_menu_item_new_with_label(menu_string);
        _menu_items.push_back(item);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu_stack[_menu_tree[i].level - 1]), item);

        if (_menu_tree[i + 1].level > _menu_tree[i].level) {
            menu_stack[_menu_tree[i].level] = gtk_menu_new();
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu_stack[_menu_tree[i].level]);
        } else {
            if (_callback != nullptr) {
                g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_callback), ON_ITEM(_instance, _menu_tree[i].id));
            }
        }
    }

    return (menu_bar);
}

void App::_on_menu(GtkApplication* gtk, void* callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(callback_parameter);
    OBJ_PTR(App, cbp->get_this())->on_menu(cbp);
}
void App::on_menu(CallbackParameter* p) {
    int item_id = p->get_item_id();

    switch (item_id) {
        case IDS_QUIT: {
            gtk_main_quit();
        } break;

        case IDS_COPY: {
        } break;

        case IDS_PASTE: {
        } break;


        // case IDS_DEVICE_MENU: {
        // } break;

        // case IDS_CONTRAST: {  
        // } break;

        // case IDS_DISPLAY_OFF: {
        // } break;

        case IDS_MAIN: {       
            app_menu_display_messagebox("In the device, this always returns to the main menu.");
        } break;

        case IDS_EXIT: {       
            app_menu_display_messagebox("In the device, this always closes the menu.");
        } break;

        case IDS_TITLE_MAIN: { 
            app_menu_display_messagebox("In the device, this is the 1st menu displayed.");
        } break;

        case IDS_LAYOUT: {     
            app_menu_display_messagebox("In the device, this is the menu for selecting the display layout.");
        } break;

        case IDS_CONFIG: {     
            app_menu_display_messagebox("In the device, this is the 1st menu displayed.");
        } break;

        case IDS_REBOOT: {     
            gdk_threads_add_idle(App::_idle_task, ON_ITEM(this, IDS_RESET_DEVICE));
        } break;

        case IDS_FACTORY_RESET: {
            gdk_threads_add_idle(App::_idle_task, ON_ITEM(this, IDS_INITIALIZE_DEVICE));
        } break;

        case IDS_TITLE_DISPLAY: {
            app_menu_display_messagebox("In the device, this menu is for display related settings.");
        } break;

        case IDS_ROTATE: {
            app_menu_display_rotation();
        } break;

        case IDS_TITLE_CONFIG: {
            app_menu_display_messagebox("This menu contains the general configuration properties.");
        } break;

        case IDS_DISPLAY: {
            app_menu_display_messagebox("This menu contains the display related configuration properties.");
        } break;

        case IDS_INTENSITY: {
            app_menu_display_messagebox("This menu is used to change the signal LED intensity.");
        } break;

        case IDS_MQTT_CLIENT: {
            app_menu_enable_disable(IDS_MQTT_ENABLE, JSON_KEY_MQTT_ENABLE);
        } break;

        case IDS_CONFIG_INTERFACE: {
            app_menu_display_messagebox("This menu is used to enable or disable the config interface.\nThis cannot be disabled while using it.\nPlease use device internal menu.");
        } break;

        case IDS_SENSOR_SELECT: {
            app_menu_display_messagebox("This menu is used to select one of the connected saensor heads.");
        } break;

        case IDS_TITLE_CONTRAST: {
            app_menu_display_messagebox("This menu is used to change the OLED display intensity.");
        } break;

        case IDS_CONTRAST_100:
        case IDS_CONTRAST_80:
        case IDS_CONTRAST_60:
        case IDS_CONTRAST_50:
        case IDS_CONTRAST_40:
        case IDS_CONTRAST_30:
        case IDS_CONTRAST_20:
        case IDS_CONTRAST_10: {
            app_menu_display_contrast(item_id);
        } break;

        case IDS_TITLE_LAYOUT: {    
        } break;

        case IDS_LAYOUT_VALUE_PAGE:
        case IDS_LAYOUT_DETAILS_PAGE:
        case IDS_LAYOUT_INFO_PAGE: {
            app_menu_display_layout(item_id);
        } break;

        case IDS_TITLE_TIMEOUT: {
        } break;

        case IDS_DISPLAY_OFF_NEVER:  
        case IDS_DISPLAY_OFF_10SEC:  
        case IDS_DISPLAY_OFF_1MIN:   
        case IDS_DISPLAY_OFF_5MIN:   
        case IDS_DISPLAY_OFF_15MIN:  
        case IDS_DISPLAY_OFF_30MIN: {
            app_menu_display_timeout(item_id);
        } break;

        case IDS_TITLE_LED_INTENSITY: {
        } break;

        case IDS_LED_INTENSITY_100:  
        case IDS_LED_INTENSITY_75:   
        case IDS_LED_INTENSITY_50:   
        case IDS_LED_INTENSITY_25:   
        case IDS_LED_INTENSITY_10:   
        case IDS_LED_INTENSITY_1: {
            app_menu_led_intensity(item_id);
        } break;

        default: {
        } break;
    }
}

void App::app_menu_display_messagebox(const char* _string) {
    GtkWidget *dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, _string, "");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void App::app_menu_display_rotation(void) {
    DialogItem* item = get_item(IDS_DISPLAY_ROTATION);
    if (item == nullptr) { return; }
    int angle = (strstr(item->field, "180") != nullptr) ? 0 : 180;
    snprintf(item->field, sizeof(item->field), "%d°", angle);
    handle_item_change(item, true);
    snprintf(m.cmd, sizeof(m.cmd), "/par=display_rotation:%d", angle);
    gdk_threads_add_idle(App::_idle_task, ON_ITEM_PAR(this, IDS_EXEC_COMMAND, m.cmd));
}

void App::app_menu_handle_str_tab(int _item_id, int _base_id, int _dlg_item_id, const char* _tab, const char* _key) {
    char string[1024]{ 0 };
    strncpy(string, _tab, sizeof (string) - 1);

    char* tab[64]{ nullptr };
    tab[0] = string;
    int len = 1;
    size_t size = strlen(string);
    for (int i = 1; i < size; i++) {
        if (string[i] == '\n') {
            string[i++] = '\0';
            tab[len++] = &string[i];
        }
    }

    DialogItem* item = get_item(_dlg_item_id);
    if (item == nullptr) { return; }

    int index = _item_id - _base_id;
    if ((index < 0) || (index >= len)) {
        return;
    }

    memset(item->field, 0, item->length);
    strncpy(item->field, tab[index], item->length);
    handle_item_change(item, true);

    snprintf(m.cmd, sizeof(m.cmd), "/par=%s:%s", _key, tab[index]);
    gdk_threads_add_idle(App::_idle_task, ON_ITEM_PAR(this, IDS_EXEC_COMMAND, m.cmd));
}

void App::app_menu_display_contrast(int _item_id) {
    app_menu_handle_str_tab(_item_id, IDS_CONTRAST_100, IDS_DISPLAY_CONTRAST, APPSTRING(IDS_LIST_DISPLAY_CONTRAST), JSON_KEY_DISPLAY_CONTRAST);
}

void App::app_menu_display_timeout(int _item_id) {
    app_menu_handle_str_tab(_item_id, IDS_DISPLAY_OFF_NEVER, IDS_DISPLAY_TIMEOUT, APPSTRING(IDS_LIST_DISPLAY_TIMEOUTS), JSON_KEY_DISPLAY_TIMEOUT);
}

void App::app_menu_display_layout(int _item_id) {
    app_menu_handle_str_tab(_item_id, IDS_LAYOUT_VALUE_PAGE, IDS_DISPLAY_LAYOUT, APPSTRING(IDS_LIST_DISPLAY_PAGE), JSON_KEY_DISPLAY_LAYOUT);
}

void App::app_menu_led_intensity(int _item_id) {
    app_menu_handle_str_tab(_item_id, IDS_LED_INTENSITY_100, IDS_LED_INTENSITY, APPSTRING(IDS_LIST_LED_INTENSITY), JSON_KEY_LED_INTENSITY);
}

void App::app_menu_enable_disable(int _dlg_item_id, const char* _key) {
    DialogItem* item = get_item(_dlg_item_id);
    if (item == nullptr) { return; }

    const char* p;
    if (strstr(item->field, APPSTRING(IDS_ENABLED)) != nullptr) {
        p = APPSTRING(IDS_DISABLED);
    } else {
        p = APPSTRING(IDS_ENABLED);
    }
    memset(item->field, 0, item->length);
    strncpy(item->field, p, item->length - 1);
    handle_item_change(item, true);

    snprintf(m.cmd, sizeof(m.cmd), "/par=%s:%s", _key, p);
    gdk_threads_add_idle(App::_idle_task, ON_ITEM_PAR(this, IDS_EXEC_COMMAND, m.cmd));
}
