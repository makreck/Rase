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

#define DISPLAY_STATE

void Mqtt::init(const char* _broker_url, const char* _username, const char* _password) {
    if (_broker_url != nullptr) {
        snprintf(m.broker_uri, sizeof (m.broker_uri), "mqtt://%s:1883", _broker_url);

        if (_username != nullptr) {
            memset(m.username, 0, sizeof(m.username));
            strncpy(m.username, _username, sizeof (m.username) - 1);
        }

        if (_password != nullptr) {
            memset(m.password, 0, sizeof(m.password));
            strncpy(m.password, _password, sizeof (m.password) - 1);
        }

        memset(&m.mqtt_cfg, 0, sizeof (m.mqtt_cfg));
        m.mqtt_cfg.broker.address.uri                  = m.broker_uri;
        m.mqtt_cfg.session.protocol_ver                = MQTT_PROTOCOL_V_5;
        m.mqtt_cfg.credentials.username                = m.username;
        m.mqtt_cfg.credentials.authentication.password = m.password;
        m.mqtt_cfg.credentials.client_id               = SENSOR_ID;

#ifdef DISPLAY_STATE
        ESP_LOGI(TAG, "Setup MQTT client for URI <%s> user = <%s> pwd = <%s>", m.broker_uri, m.username, m.password);
#endif
    }
}

void Mqtt::cleanup(void) {
    if (m.client != nullptr) {
        esp_mqtt_client_stop(m.client);
        esp_mqtt_client_destroy(m.client);
        m.client = nullptr;
    }
}

void Mqtt::start(void) {
    if (m.client == nullptr) {
#ifdef DISPLAY_STATE
        ESP_LOGI(TAG, "Init MQTT client at URI <%s> user = <%s> pwd = <%s>", m.broker_uri, m.username, m.password);
#endif
        m.client = esp_mqtt_client_init(&m.mqtt_cfg);
        if (m.client != nullptr) {
            esp_mqtt_client_register_event(m.client, MQTT_EVENT_ANY, Mqtt::_mqtt_event_handler, this);
        } else {
            ESP_LOGE(TAG, "Failed to create MQTT client");
            return;
        }
    }

    esp_err_t ret = esp_mqtt_client_start(m.client);
    if (ret == ESP_OK) {
#ifdef DISPLAY_STATE
        ESP_LOGI(TAG, "MQTT client is running at URI <%s> user = <%s> pwd = <%s>, client handle 0x%-8.8lX", m.broker_uri, m.username, m.password, (unsigned long)m.client);
#endif
    } else {
        ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(ret));
    }
}

void Mqtt::stop(void) {
    if (m.client != nullptr) {
        esp_mqtt_client_stop(m.client);
    }
}

void Mqtt::_mqtt_event_handler(void* _handler_args, esp_event_base_t _base, int32_t _event_id, void* _event_data) {
    (reinterpret_cast<Mqtt*>(_handler_args))->mqtt_event_handler(_base, _event_id, _event_data);
}
void Mqtt::mqtt_event_handler(esp_event_base_t _base, int32_t _event_id, void* _event_data) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "MQTT event ID #%d", (unsigned int)_event_id);
#endif
    switch (_event_id) {
        case MQTT_EVENT_CONNECTED: {
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        } break;

        case MQTT_EVENT_DISCONNECTED: {
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        } break;

        case MQTT_EVENT_SUBSCRIBED: {
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
        } break;

        case MQTT_EVENT_UNSUBSCRIBED: {
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        } break;

        case MQTT_EVENT_PUBLISHED: {
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
        } break;

        case MQTT_EVENT_DATA: {
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        } break;

        case MQTT_EVENT_ERROR: {
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
            if (_event_data != nullptr) {
                esp_mqtt_error_codes_t *error = (esp_mqtt_error_codes_t *)_event_data;
                ESP_LOGE(TAG, "%d, reported from esp-tls", error->esp_tls_last_esp_err);
                ESP_LOGE(TAG, "%d, reported from mbedtls", error->esp_tls_stack_err);
                ESP_LOGE(TAG, "%d, captured as transport's socket errno", error->esp_transport_sock_errno);
            }
        } break;

        default: {
            ESP_LOGI(TAG, "Other event id: %d ", (int)_event_id);
        } break;
    }
}
