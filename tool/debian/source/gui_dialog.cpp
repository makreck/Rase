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

GtkWidget* App::create_dialog(void) {
    m.gtk.dialog = gtk_frame_new(nullptr);
    gtk_frame_set_shadow_type(GTK_FRAME(m.gtk.dialog), GTK_SHADOW_IN);

    m.gtk.scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width((GtkContainer *)m.gtk.scrolled, 4);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *)m.gtk.scrolled, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(m.gtk.dialog), m.gtk.scrolled);

    GtkWidget* box = gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(m.gtk.scrolled), box);

    GtkWidget* grid_wifi = add_grid(APPSTRING(IDS_BOX_WIFI_CONFIG), box);
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_SSID,        APP_WINDOW_LONG_WIDTH,  0, 0, m.device.cfg.wifi_ssid,         sizeof (m.device.cfg.wifi_ssid)));
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_PASSWORD,    APP_WINDOW_LONG_WIDTH,  2, 0, m.device.cfg.wifi_password,     sizeof (m.device.cfg.wifi_password)));
    m.gtk.items.push_back(add_text_field(grid_wifi, IDS_WIFI_CHANNEL,     APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.cfg.wifi_channel,      sizeof (m.device.cfg.wifi_channel)));

    GtkWidget* grid_mqtt = add_grid(APPSTRING(IDS_BOX_MQTT_CONFIG), box);
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_BROKER,      APP_WINDOW_LONG_WIDTH,  0, 0, m.device.cfg.mqtt_broker,       sizeof (m.device.cfg.mqtt_broker)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_ENABLE,      APP_WINDOW_SHORT_WIDTH, 2, 0, m.device.cfg.mqtt_enable,       sizeof (m.device.cfg.mqtt_enable),       APPSTRING(IDS_LIST_ENABLE_DISABLE)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_USERNAME,    APP_WINDOW_LONG_WIDTH,  0, 1, m.device.cfg.mqtt_username,     sizeof (m.device.cfg.mqtt_username)));
    m.gtk.items.push_back(add_text_field(grid_mqtt, IDS_MQTT_PASSWORD,    APP_WINDOW_LONG_WIDTH,  2, 1, m.device.cfg.mqtt_password,     sizeof (m.device.cfg.mqtt_password)));

    GtkWidget* grid_oled = add_grid(APPSTRING(IDS_BOX_DISPLAY_CONFIG), box);
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_TIMEOUT,  APP_WINDOW_SHORT_WIDTH, 0, 0, m.device.cfg.display_timeout_s, sizeof (m.device.cfg.display_timeout_s), APPSTRING(IDS_LIST_DISPLAY_TIMEOUTS)));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_CONTRAST, APP_WINDOW_SHORT_WIDTH, 2, 0, m.device.cfg.display_contrast,  sizeof (m.device.cfg.display_contrast),  APPSTRING(IDS_LIST_DISPLAY_CONTRAST)));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_ROTATION, APP_WINDOW_SHORT_WIDTH, 4, 0, m.device.cfg.display_rotoation, sizeof (m.device.cfg.display_rotoation), APPSTRING(IDS_LIST_DISPLAY_ROTATION)));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_LAYOUT,   APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.cfg.display_layout,    sizeof (m.device.cfg.display_layout),    APPSTRING(IDS_LIST_DISPLAY_PAGE)));
    m.gtk.items.push_back(add_text_field(grid_oled, IDS_DISPLAY_PARAM,    APP_WINDOW_SHORT_WIDTH, 2, 1, m.device.cfg.display_param,     sizeof (m.device.cfg.display_param)));

    GtkWidget* grid_misc = add_grid(APPSTRING(IDS_BOX_MISCELLANEOUS), box);
    m.gtk.items.push_back(add_text_field(grid_misc, IDS_LED_INTENSITY,    APP_WINDOW_SHORT_WIDTH, 0, 0, m.device.cfg.led_intensity,     sizeof (m.device.cfg.led_intensity),     APPSTRING(IDS_LIST_LED_INTENSITY)));
    m.gtk.items.push_back(add_text_field(grid_misc, IDS_SENSOR_TYPE,      APP_WINDOW_SHORT_WIDTH, 0, 1, m.device.cfg.sensor_type,       sizeof (m.device.cfg.sensor_type),       m.device.cfg._sensor_type_list));

    DialogItem* item;
    item = get_item(IDS_WIFI_PASSWORD);
    if (item != nullptr) {
        gtk_entry_set_visibility(GTK_ENTRY(item->widget), FALSE);
    }
    item = get_item(IDS_MQTT_PASSWORD);
    if (item != nullptr) {
        gtk_entry_set_visibility(GTK_ENTRY(item->widget), FALSE);
    }

    
    GtkWidget* grid_id = add_grid(APPSTRING(IDS_BOX_ID), box);
    m.gtk.items.push_back(add_text_field(grid_id, IDS_IDENTIFICATION,   APP_WINDOW_INFO_WIDTH, 0, 0, m.device.id.identification,       sizeof (m.device.id.identification),       nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_MANUFACTURER,     APP_WINDOW_INFO_WIDTH, 2, 0, m.device.id.manufacturer,         sizeof (m.device.id.manufacturer),         nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_PRODUCT,          APP_WINDOW_INFO_WIDTH, 0, 1, m.device.id.product,              sizeof (m.device.id.product),              nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_SERIAL_NUMBER,    APP_WINDOW_INFO_WIDTH, 2, 1, m.device.id.device_serial_number, sizeof (m.device.id.device_serial_number), nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_FIRMWARE_VERSION, APP_WINDOW_INFO_WIDTH, 0, 2, m.device.id.firmware_version,     sizeof (m.device.id.firmware_version),     nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_FIRMWARE_DATE,    APP_WINDOW_INFO_WIDTH, 2, 2, m.device.id.firmware_date,        sizeof (m.device.id.firmware_date),        nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_IP_ADDRESS,       APP_WINDOW_INFO_WIDTH, 0, 3, m.device.id.ip_addr,              sizeof (m.device.id.ip_addr),              nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_CHIP_TYPE,        APP_WINDOW_INFO_WIDTH, 2, 3, m.device.id.chip_type,            sizeof (m.device.id.chip_type),            nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_SENSOR_HEAD,      APP_WINDOW_INFO_WIDTH, 0, 4, m.device.id.head,                 sizeof (m.device.id.head),                 nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_HEAD_SERIAL_NO,   APP_WINDOW_INFO_WIDTH, 2, 4, m.device.id.head_serial,          sizeof (m.device.id.head_serial),          nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_WIFI_RSSI,        APP_WINDOW_INFO_WIDTH, 0, 5, m.device.id.rssi,                 sizeof (m.device.id.rssi),                 nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_WIFI_TX_POWER,    APP_WINDOW_INFO_WIDTH, 2, 5, m.device.id.tx_power,             sizeof (m.device.id.tx_power),             nullptr));

    m.gtk.items.push_back(add_text_field(grid_id, IDS_WIFI_STATION_MAC, APP_WINDOW_INFO_WIDTH, 0, 6, m.device.id.wifi_station_mac,     sizeof (m.device.id.wifi_station_mac),     nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_WIFI_AP_MAC,      APP_WINDOW_INFO_WIDTH, 2, 6, m.device.id.wifi_ap_mac,          sizeof (m.device.id.wifi_ap_mac),          nullptr));
    
    m.gtk.items.push_back(add_text_field(grid_id, IDS_BLUETOOTH_MAC,    APP_WINDOW_INFO_WIDTH, 0, 7, m.device.id.bluetooth_mac,        sizeof (m.device.id.bluetooth_mac),        nullptr));
    m.gtk.items.push_back(add_text_field(grid_id, IDS_TIME_DATE,        APP_WINDOW_INFO_WIDTH, 2, 7, m.device.id.system_time,          sizeof (m.device.id.system_time),          nullptr));

    return (m.gtk.dialog);
}

DialogItem* App::get_item(int _item_id) {
    for (DialogItem*& item : m.gtk.items) {
        if (item != nullptr) {
            if (item->id == _item_id) {
                return (item);
            }
        }
    }
    return (nullptr);
}

void App::handle_item_change(DialogItem *_item, bool _setup) {
    if ((_item->field != nullptr) && (_item->widget != nullptr)) {
        if (_setup) {
            if (_item->list == nullptr) {
                gtk_entry_set_text(GTK_ENTRY(_item->widget), _item->field);
            } else {
                App::string_combobox_setup(_item->widget, _item->field, _item->list);
            }
        } else {
            if (_item->list == nullptr) {
                const gchar* item_text = gtk_entry_get_text(GTK_ENTRY(_item->widget));
                if (item_text != nullptr) {
                    memset(_item->field, 0, _item->length);
                    strncpy(_item->field, item_text, _item->length - 1);
                }
            } else {
                gchar* item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(_item->widget));
                if (item_text != nullptr) {
                    memset(_item->field, 0, _item->length);
                    strncpy(_item->field, item_text, _item->length - 1);
                    g_free(item_text);
                }
            }
        }
    }
}

void App::handle_dialog_items(bool _setup) {
    for (DialogItem*& item : m.gtk.items) {
        if (item != nullptr) {
            handle_item_change(item, _setup);
        }
    }
}

void App::_on_command(GtkApplication* gtk, void* callback_parameter) {
    CallbackParameter* cbp = CALLBACK_PARAMETER(callback_parameter);
    OBJ_PTR(App, cbp->get_this())->on_command(cbp);
}
void App::on_command(CallbackParameter* p) {
    int item_id = p->get_item_id();
    DialogItem *item = get_item(item_id);
    if (item != nullptr) {
        handle_item_change(item, false);
    }
}
