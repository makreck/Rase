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

void Mqtt::init(const char* _broker_url, const char* _username, const char* _password) {
    if (_broker_url != nullptr) {
        snprintf(m.broker_uri, sizeof (m.broker_uri), "mqtt://%s", _broker_url);

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
        m.mqtt_cfg.broker.address.port                 = 1883;
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
#ifdef DISPLAY_STATE
        ESP_LOGI(TAG, "Cleanup MQTT client for URI <%s> user = <%s> pwd = <%s>", m.broker_uri, m.username, m.password);
#endif
    if (m.task_handle != nullptr) {
        vTaskDelete(m.task_handle);
        m.task_handle = nullptr;
    }

    if (m.client != nullptr) {
        esp_mqtt_client_stop(m.client);
        esp_mqtt_client_destroy(m.client);
        m.client = nullptr;
    }
}

void Mqtt::start(SensorDriver* _sensor) {
    m.sensor = _sensor;

    if (m.client == nullptr) {
        m.client = esp_mqtt_client_init(&m.mqtt_cfg);
        if (m.client != nullptr) {
            esp_mqtt_client_register_event(m.client, MQTT_EVENT_ANY, Mqtt::_mqtt_event_handler, this);
        } else {
            ESP_LOGE(TAG, "Failed to create MQTT client");
            return;
        }
    }

    if (m.task_handle == nullptr) {
        esp_err_t ret = esp_mqtt_client_start(m.client);
        if (ret == ESP_OK) {
            xTaskCreate(Mqtt::_mqtt_task, "MQTTClientTask", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY - 1, &m.task_handle);
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, " MQTT client started for URI <%s> user = <%s> pwd = <%s>", m.broker_uri, m.username, m.password);
#endif
        }
    }
}

void Mqtt::stop(void) {
    if (m.task_handle != nullptr) {
        vTaskDelete(m.task_handle);
        m.task_handle = nullptr;
    }

    if (m.client != nullptr) {
        esp_mqtt_client_stop(m.client);
    }
}

void Mqtt::_mqtt_event_handler(void* _handler_args, esp_event_base_t _base, int32_t _event_id, void* _event_data) {
    (reinterpret_cast<Mqtt*>(_handler_args))->mqtt_event_handler(_base, _event_id, _event_data);
}
void Mqtt::mqtt_event_handler(esp_event_base_t _base, int32_t _event_id, void* _event_data) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "MQTT event base=\"%s\" ID #%d", _base, (unsigned int)_event_id);
#endif
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)_event_data;

    esp_mqtt_client_handle_t client = event->client;
    esp_mqtt_event_id_t event_id = (esp_mqtt_event_id_t)_event_id;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
#endif
            m.retry_count = 0;
            subscribe_sensor(client);
        }
        break;

        case MQTT_EVENT_DISCONNECTED: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
#endif
            m.retry_count++;
            if (m.retry_count < MQTT_RETRY_MAX) {
                esp_mqtt_client_reconnect(client);
                ESP_LOGI(TAG, "Retrying to connect to the MQTT broker");
            } else {
                ESP_LOGI(TAG, "Maximum retry attempts reached");
            }
        } break;

        case MQTT_EVENT_SUBSCRIBED: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
#endif
        } break;

        case MQTT_EVENT_UNSUBSCRIBED: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
#endif
        } break;

        case MQTT_EVENT_PUBLISHED: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
#endif
        } break;

        case MQTT_EVENT_DATA: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
#endif
        } break;

        case MQTT_EVENT_ERROR: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                ESP_LOGI(TAG, "Last error code reported from tcp transport = 0x%X, last tls stack error number: 0x%X",
                    event->error_handle->esp_tls_last_esp_err, event->error_handle->esp_tls_stack_err);
            }
#endif
        } break;

        default: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Unknown MQTT event");
#endif
        } break;

    }
}

void Mqtt::subscribe_sensor(esp_mqtt_client_handle_t _client) {
    if (m.sensor != nullptr) {
        size_t count = m.sensor->get_property_count();
        if (count > 0) {
            char device_serial_number[24]{ 0 };
            Tools::get_device_serial_number(device_serial_number, sizeof (device_serial_number));

            const SensorProperty* props = m.sensor->get_properties();
            for (int i = 0; i < count; i++) {
                char path[256]{ 0 };
                snprintf(path, sizeof (path) - 1, "/%s/%s", device_serial_number, props[i].key);
                esp_mqtt_client_subscribe(_client, path, 0);
            }

        }
    }
}

void Mqtt::perform_publishing(void) {
    if (m.sensor != nullptr) {
        size_t count = m.sensor->get_property_count();
        if (count > 0) {
            SensorReading* reading = m.sensor->get_reading();

            char device_serial_number[24]{ 0 };
            Tools::get_device_serial_number(device_serial_number, sizeof (device_serial_number));

            const SensorProperty* props = m.sensor->get_properties();
            for (int i = 0; i < count; i++) {
                char path[256]{ 0 };
                snprintf(path, sizeof (path) - 1, "/%s/%s", device_serial_number, props[i].key);
                float value = 0.0f;
                if (reading->get_Value(props[i].key, value)) {
                    char message[32]{ 0 };
                    SensorProperty::format_value(&props[i], value, message, sizeof (message));
                    esp_mqtt_client_publish(m.client, path, message, 0, 1, 0);
                }
            }
        }
    }
}

void Mqtt::_mqtt_task(void* pvParameters) {
    (reinterpret_cast<Mqtt*>(pvParameters))->mqtt_task();
}
void Mqtt::mqtt_task(void) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        perform_publishing();
    }
}
