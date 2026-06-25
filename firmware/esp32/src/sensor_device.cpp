/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    ESP32-S3 Station Device Firmware
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

#include "includes.hpp"
#include "app.hpp"

// #define DISPLAY_STATE

const char* SensorDevice::sensor_header = 
    "{\n"
    "\t\"sensor\": \"%s\",\n"
    "\t\"device_serial_number\": \"%s\",\n"
    "\t\"timestamp\": \"%s\",\n"
    "\t\"rssi\": \"%s\",\n"
    "\t\"tx_power\": \"%s\",\n"
    "\t\"channels\": %d,\n"
    "\t\"nodes\": [\n";

const char* SensorDevice::opcua_header = 
    "{\n"
    "\t\"nodes\": [\n";

const char* SensorDevice::end_of_list = 
    "\t]\n"
    "}\n";

SensorDevice::SensorDevice(const char* _device_name) {
    if (_device_name != nullptr) {
        strncpy(m.device_name, _device_name, sizeof (m.device_name));
    } else {
        strncpy(m.device_name, "FuSe.ESP32-S3", sizeof (m.device_name));
    }
    m.node.clear();
    m.mutex = xSemaphoreCreateMutex();
}

SensorDevice::~SensorDevice() {
    delete_driver();

    xSemaphoreTake(m.mutex, portMAX_DELAY);

    if (m.json != nullptr) {
        free(m.json);
        m.json = nullptr;
        m.length = 0;
    }

    if (m.opcua_json != nullptr) {
        free(m.opcua_json);
        m.opcua_json = nullptr;
        m.opcua_length = 0;
    }

    xSemaphoreGive(m.mutex);
    vSemaphoreDelete(m.mutex);
}

void SensorDevice::delete_driver(void) {
    xSemaphoreTake(m.mutex, portMAX_DELAY);

    SAFE_DELETE(m.driver);
    
    for (size_t i = 0; i < m.node.size(); i++) {
        delete (m.node[i]);
    }
    m.node.clear();

    memset(m.device_name, 0, sizeof (m.device_name));
    strcpy(m.device_name, "FuSe.None");

    xSemaphoreGive(m.mutex);
}

void SensorDevice::update_json(void) {
    xSemaphoreTake(m.mutex, portMAX_DELAY);

    size_t count = m.node.size();

    char time_string[TIME_STAMP_LENGTH]{0};
    Tools::get_timestamp(time_string, sizeof (time_string));

    char rssi_string[16]{ 0 };
    Wifi_Station::get_rssi_dbm(rssi_string, sizeof (rssi_string));

    char tx_power_string[16]{ 0 };
    Wifi_Station::get_tx_power_dbm(tx_power_string, sizeof (tx_power_string));

    char device_serial_number[22]{0};
    Tools::get_device_serial_number(device_serial_number, sizeof (device_serial_number));
    
    size_t length = snprintf(nullptr, 0, SensorDevice::sensor_header,
        m.device_name, device_serial_number, time_string, rssi_string, tx_power_string, (int)count);

    for (size_t i = 0; i < count; i++) {
        length += m.node[i]->get_length();
        if (i < (count - 1)) {
            length += strlen(",\n");
        } else {
            length += strlen("\n");
        }
    }
    length += (strlen(end_of_list) + 1);

    if (m.json != nullptr) {
#ifdef DISPLAY_STATE        
        ESP_LOGI(TAG, "Free allocated JSON memory...................");
#endif
        m.length = 0;
        free(m.json);
    }

#ifdef DISPLAY_STATE        
    ESP_LOGI(TAG, "Allocate new JSON memory...................");
#endif
    m.length = length;
    m.json = (char*)malloc(length + 8);
    memset(m.json, 0, length + 8);

    snprintf(m.json, m.length, SensorDevice::sensor_header,
        m.device_name, device_serial_number, time_string, rssi_string, tx_power_string, (int)count);
    
    for (size_t i = 0; i < count; i++) {
        strcat(m.json, m.node[i]->get_json());
        if (i < (count - 1)) {
            strcat(m.json, ",\n");
        } else {
            strcat(m.json, "\n");
        }
    }
    strncat(m.json, SensorDevice::end_of_list, m.length);

    xSemaphoreGive(m.mutex);
}

void SensorDevice::add_channel(SensorNode* channel) {
    m.node.push_back(channel);
}

SensorDriver* SensorDevice::get_driver(void) {
    return (m.driver);
}

void SensorDevice::set_driver(SensorDriver* driver) {
    if (m.driver != nullptr) {
        delete_driver();
    }

    if (driver == nullptr) {
        return;
    }
    
    m.driver = driver;
    m.driver->wait_ready();

    snprintf(m.device_name,
        sizeof (m.device_name), PRODUCT_ID " V%d.%d.%d.%d (%s)", VS_HIGH, VS_LOW, VS_REV, VS_BUILD, m.driver->get_head());

    const SensorProperty* props = m.driver->get_properties();
    size_t count = m.driver->get_property_count();
    for (size_t i = 0; i < count; i++) {
        add_channel(new SensorNode(&props[i]));
    }
}

size_t SensorDevice::get_channel_count(void) {
    return (m.node.size());
}

bool SensorDevice::get_channel_props(size_t index, SensorNodeProps& props, bool modify_unit) {
    if (index >= m.node.size()) {
        return (false);
    }
    m.node[index]->get_properties(props, modify_unit);
    return (true);
}

void SensorDevice::update(SensorReading* reading) {
    for (size_t i = 0; i < m.node.size(); i++) {
        float value = 0.0f;
        const char* key = m.node[i]->get_key();
        if (reading->get_Value(key, value)) {
            m.node[i]->add_Measurement(value);
        }
    }
}

char* SensorDevice::get_json(size_t& length) {
#ifdef DISPLAY_STATE        
    ESP_LOGI(TAG, "SensorDevice::get_json()");
#endif
    update_json();

    xSemaphoreTake(m.mutex, portMAX_DELAY);
    char* json_copy = (char*)malloc(m.length);
    memset(json_copy, 0, m.length);
    strncpy(json_copy, m.json, m.length);
    length = strlen(json_copy);
    xSemaphoreGive(m.mutex);

    return (json_copy);
}


char* SensorDevice::get_opcua_json(size_t& length) {
#ifdef DISPLAY_STATE        
    ESP_LOGI(TAG, "SensorDevice::get_opcua_json()");
#endif
    update_opcua_json();

    xSemaphoreTake(m.mutex, portMAX_DELAY);
    char* json_copy = (char*)malloc(m.opcua_length);
    memset(json_copy, 0, m.opcua_length);
    strncpy(json_copy, m.opcua_json, m.opcua_length);
    length = strlen(json_copy);
    xSemaphoreGive(m.mutex);

    return (json_copy);
}

void SensorDevice::update_opcua_json(void) {
#ifdef DISPLAY_STATE        
    ESP_LOGI(TAG, "SensorDevice::update_opcua_json()");
#endif
    xSemaphoreTake(m.mutex, portMAX_DELAY);

    size_t count = m.node.size();

    char time_string[TIME_STAMP_LENGTH]{0};
    snprintf(time_string, sizeof (time_string), "%ld", (unsigned long)Tools::get_tick_seconds());

    char device_serial_number[22]{0};
    Tools::get_device_serial_number(device_serial_number, sizeof (device_serial_number));
    
    size_t length = strlen(SensorDevice::opcua_header); // snprintf(nullptr, 0, SensorDevice::opcua_header);
    for (size_t i = 0; i < count; i++) {
        length += m.node[i]->get_opcua_length();
        if (i < (count - 1)) {
            length += strlen(",\n");
        } else {
            length += strlen("\n");
        }
    }
    length += (strlen(end_of_list) + 1);

    if (m.opcua_json != nullptr) {
#ifdef DISPLAY_STATE        
        ESP_LOGI(TAG, "Free allocated OPCUA-JSON memory...................");
#endif
        m.opcua_length = 0;
        free(m.opcua_json);
    }

#ifdef DISPLAY_STATE        
    ESP_LOGI(TAG, "Allocate new OPCUA-JSON memory...................");
#endif
    m.opcua_length = length;
    m.opcua_json = (char*)malloc(m.opcua_length + 8);
    memset(m.opcua_json, 0, length + 8);

    strncat(m.opcua_json, SensorDevice::opcua_header, m.opcua_length);
    for (size_t i = 0; i < count; i++) {
        strcat(m.opcua_json, m.node[i]->get_opcua_json());
        if (i < (count - 1)) {
            strcat(m.opcua_json, ",\n");
        } else {
            strcat(m.opcua_json, "\n");
        }
    }
    strncat(m.opcua_json, SensorDevice::end_of_list, m.opcua_length);

    xSemaphoreGive(m.mutex);
}
