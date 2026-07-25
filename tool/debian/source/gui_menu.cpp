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

        case IDS_MAIN: {       
            GtkWidget* dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "In the device, this always returns to the main menu.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } break;

        case IDS_EXIT: {       
            GtkWidget* dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "In the device, this always closes the menu.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } break;

        case IDS_TITLE_MAIN: { 
            GtkWidget* dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "In the device, this is the 1st menu displayed.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } break;

        // case IDS_LAYOUT: {     
        // } break;

        case IDS_CONFIG: {     
            GtkWidget* dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "In the device, this is the 1st menu displayed.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } break;

        case IDS_REBOOT: {     
            gdk_threads_add_idle(App::_idle_task, ON_ITEM(this, IDS_RESET_DEVICE));
        } break;

        case IDS_FACTORY_RESET: {
            gdk_threads_add_idle(App::_idle_task, ON_ITEM(this, IDS_INITIALIZE_DEVICE));
        } break;

        case IDS_TITLE_DISPLAY: {
            GtkWidget* dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "In the device, this menu is for display related settings.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } break;

        case IDS_ROTATE: {
            app_menu_display_rotation();
        } break;

        // case IDS_CONTRAST: {  
        // } break;

        // case IDS_DISPLAY_OFF: {
        // } break;

        case IDS_TITLE_CONFIG: {
        } break;

        case IDS_DISPLAY: {
        } break;

        case IDS_INTENSITY: {
        } break;

        case IDS_MQTT_CLIENT: {
        } break;

        case IDS_CONFIG_INTERFACE: {
        } break;

        case IDS_SENSOR_SELECT: {
        } break;

        case IDS_TITLE_CONTRAST: {
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
        } break;

        case IDS_TITLE_TIMEOUT: {
        } break;

        case IDS_DISPLAY_OFF_NEVER:  
        case IDS_DISPLAY_OFF_10SEC:  
        case IDS_DISPLAY_OFF_1MIN:   
        case IDS_DISPLAY_OFF_5MIN:   
        case IDS_DISPLAY_OFF_15MIN:  
        case IDS_DISPLAY_OFF_30MIN: {
        } break;

        case IDS_TITLE_LED_INTENSITY: {
        } break;

        case IDS_LED_INTENSITY_100:  
        case IDS_LED_INTENSITY_75:   
        case IDS_LED_INTENSITY_50:   
        case IDS_LED_INTENSITY_25:   
        case IDS_LED_INTENSITY_10:   
        case IDS_LED_INTENSITY_1: {
        } break;

        default: {
        } break;
    }
}

void App::app_menu_display_rotation(void) {
    DialogItem *item = get_item(IDS_DISPLAY_ROTATION);
    if (item == nullptr) { return; }
    int angle = (strstr(item->field, "180") != nullptr) ? 0 : 180;
    snprintf(item->field, sizeof(item->field), "%d°", angle);
    handle_item_change(item, true);
    snprintf(m.cmd, sizeof(m.cmd), "/par=display_rotation:%d", angle);
    gdk_threads_add_idle(App::_idle_task, ON_ITEM_PAR(this, IDS_EXEC_COMMAND, m.cmd));
}

void App::app_menu_display_contrast(int _item_id) {
    DialogItem *item = get_item(IDS_DISPLAY_CONTRAST);
    if (item == nullptr) { return; }

    int tab[] = { 100, 80, 60, 50, 40, 30, 20, 10 };
    int index = _item_id - IDS_CONTRAST_100;
    if ((index < 0) || (index >= SIZEOFARRAY(tab))) {
        return;
    }
    
    snprintf(item->field, sizeof(item->field), "%d%%", tab[index]);
    handle_item_change(item, true);

    snprintf(m.cmd, sizeof(m.cmd), "/par=display_contrast:%d", tab[index]);
    gdk_threads_add_idle(App::_idle_task, ON_ITEM_PAR(this, IDS_EXEC_COMMAND, m.cmd));
}
