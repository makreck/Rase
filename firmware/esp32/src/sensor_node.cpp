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

#include "app.hpp"

// #define DISPLAY_STATE

const char* SensorNode::json_format =
    "\t\t{ \"%s\": {\n"
    "\t\t\t\"name\": \"%s\",\n"
    "\t\t\t\"shortcut\": \"%s\",\n"
    "\t\t\t\"unit\": \"%s\",\n"
    "\t\t\t\"color\": \"%s\",\n"
    "\t\t\t\"bottom\": %.3f,\n"
    "\t\t\t\"top\": %.3f,\n"
    "\t\t\t\"value\": %.3f,\n"
    "\t\t\t\"min\": %.3f,\n"
    "\t\t\t\"max\": %.3f,\n"
    "\t\t\t\"average\": %.3f,\n"
    "\t\t\t\"flags\": %d,\n"
    "\t\t\t\"count\": %d,\n"
    "\t\t\t\"history\": [ %s ] }\n"
    "\t\t}\n";

const char* SensorNode::opcua_json_format =
    "\t\t{\n"
    "\t\t\t\"nodeId\" : \"ns=1;i=%d\",\n"
    "\t\t\t\"browseName\" : \"%s\",\n"
    "\t\t\t\"value\" : %.3f,\n"
    "\t\t\t\"dataType\" : \"Double\",\n"
    "\t\t\t\"timestamp\" : %s\n"
    "\t\t}\n";


SensorNode::SensorNode(const SensorProperty* properties) {
    init(properties->key, properties->name, properties->shortcut, properties->unit,
        properties->color, properties->bottom, properties->top, properties->flags, properties->node_id);
}

SensorNode::~SensorNode() {
    clear();
    vSemaphoreDelete(m.mutex);
}

void SensorNode::init(const char *_key, const char *_name, const char *_shortcut, const char *_unit,
    const char* _color, float _bottom, float _top, uint32_t _flags, int _node_id) {

    m.key      = _key;
    m.name     = _name;
    m.shortcut = _shortcut;
    m.unit     = _unit;
    m.color    = _color;
    m.bottom   = _bottom;
    m.top      = _top;
    m.flags    = _flags;
    
    m.value    = _bottom;
    m.min      = _top;
    m.max      = _bottom;
    m.node_id  = _node_id;

    m.mutex    = xSemaphoreCreateMutex();
}

void SensorNode::clear(void) {
    xSemaphoreTake(m.mutex, portMAX_DELAY);

    m.length = 0;
    if (m.json != nullptr) {
#ifdef DISPLAY_STATE        
        ESP_LOGI(TAG, "Free allocated channel JSON memory .....");
#endif
        free(m.json);
        m.json = nullptr;
    }

    m.opcua_length = 0;
    if (m.opcua_json != nullptr) {
#ifdef DISPLAY_STATE        
        ESP_LOGI(TAG, "Free allocated channel OPC-UA JSON memory .....");
#endif
        free(m.opcua_json);
        m.opcua_json = nullptr;
    }

    xSemaphoreGive(m.mutex);
}

void SensorNode::reset(void) {
    m.value = m.bottom;
    m.min   = m.top;
    m.max   = m.bottom;
    m.sum   = 0.0f;
    m.count = 0;
    
    m.history.clear();
    clear();
}

const char* SensorNode::get_json(void) {
    update();
    return (m.json);
}

size_t SensorNode::get_length(void) {
    update();
    return (m.length);
}

void SensorNode::update(void) {
    if (m.json != nullptr) {
        return;
    }

    xSemaphoreTake(m.mutex, portMAX_DELAY);

    char* history_list = m.history.to_string();
    float average = (m.count > 0.0f) ? (m.sum / m.count) : 0.0f;

    m.length = snprintf(nullptr, 0, SensorNode::json_format,
        m.key, m.name, m.shortcut, m.unit, m.color, m.bottom, m.top, m.value, m.min, m.max, average, m.flags, (unsigned int)m.count, history_list);

#ifdef DISPLAY_STATE        
    ESP_LOGI(TAG, "Allocate new channel JSON memory .....");
#endif
    m.json = (char *)malloc(m.length + 8);
    memset(m.json, 0, m.length + 8);
    
    snprintf(m.json, m.length, SensorNode::json_format,
        m.key, m.name, m.shortcut, m.unit, m.color, m.bottom, m.top, m.value, m.min, m.max, average, m.flags, (unsigned int)m.count, history_list);

    free(history_list);
    
    xSemaphoreGive(m.mutex);
}

const char* SensorNode::get_opcua_json(void) {
    update_opcua();
    return (m.opcua_json);
}

size_t SensorNode::get_opcua_length(void) {
    update_opcua();
    return (m.opcua_length);
}

void SensorNode::update_opcua(void) {
    if (m.json != nullptr) {
        return;
    }

    char time_string[TIME_STAMP_LENGTH]{0};
    snprintf(time_string, sizeof (time_string), "%ld", (unsigned long)Tools::get_tick_seconds());

#ifdef DISPLAY_STATE        
    ESP_LOGI(TAG, "Allocate new OPC-UA JSON node memory .....");
#endif

    xSemaphoreTake(m.mutex, portMAX_DELAY);
    
    m.opcua_length = snprintf(nullptr, 0, SensorNode::opcua_json_format, m.node_id, m.name, m.value, time_string);
    m.opcua_json = (char *)malloc(m.opcua_length + 8);
    memset(m.opcua_json, 0, m.opcua_length + 8);
    snprintf(m.opcua_json, m.opcua_length, SensorNode::opcua_json_format, m.node_id, m.name, m.value, time_string);
    
    xSemaphoreGive(m.mutex);
}

void SensorNode::get_properties(SensorNodeProps& props, bool modify_unit) {
    memset(&props, 0, sizeof (props));
    strncpy(props.name, m.name, sizeof (props.name));
    strncpy(props.shortcut, m.shortcut, 3);
    if (strstr(m.unit, "°C") != nullptr) {
        strncpy(props.unit, "'C", sizeof (props.unit));
    } else if (strstr(m.unit, "g/m³") != nullptr) {
        if (modify_unit) {
            strncpy(props.unit, "gr", sizeof (props.unit));
        } else {
            strncpy(props.unit, "g/m3", sizeof (props.unit));
        }
    } else {
        strncpy(props.unit, m.unit, MIN(strlen(m.unit), sizeof (props.unit)));
    }
    props.bottom = m.bottom;
    props.top = m.top;
    props.value = m.value;
    props.min = m.min;
    props.max = m.max;
    if (props.count > 0) {
        props.average = m.sum / (float)m.count;
    }
    props.count = 0.0f;
}

AppState SensorNode::add_Measurement(float value) {
    if (isnan(value)) return (AppState::invalid_arg);
    
    value = std::max(m.bottom, std::min(m.top, value));

    if (m.count == 0) {
        m.min = value;
        m.max = value;        
    } else {
        m.min = std::min(m.min, value);
        m.max = std::max(m.max, value);
    }

    m.value  = value;
    m.sum   += value;

    m.history.add_value(value);

    m.count += 1;

    clear();

    return (AppState::OK);
}
