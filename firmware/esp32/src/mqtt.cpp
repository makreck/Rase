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

        char sernum[24]{ 0 };
        Tools::get_device_serial_number(sernum, sizeof (sernum));
        snprintf(m.client_id, sizeof (m.client_id) - 1, "%s_%s", SENSOR_ID, sernum);

        memset(&m.mqtt_cfg, 0, sizeof (m.mqtt_cfg));
        m.mqtt_cfg.broker.address.uri                  = m.broker_uri;
        m.mqtt_cfg.broker.address.port                 = 1883;
        m.mqtt_cfg.session.protocol_ver                = MQTT_PROTOCOL_V_3_1_1; // MQTT_PROTOCOL_V_5;
        m.mqtt_cfg.session.keepalive                   = 120;
        m.mqtt_cfg.session.disable_keepalive           = true;
        m.mqtt_cfg.credentials.username                = m.username;
        m.mqtt_cfg.credentials.authentication.password = m.password;
        m.mqtt_cfg.credentials.client_id               = m.client_id;
        m.mqtt_cfg.network.reconnect_timeout_ms        = 10000;
    }
}

void Mqtt::cleanup(void) {
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
            return;
        }
    }

    if (m.task_handle == nullptr) {
        esp_err_t ret = esp_mqtt_client_start(m.client);
        if (ret == ESP_OK) {
            xTaskCreate(Mqtt::_mqtt_task, "MQTTClientTask", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY - 1, &m.task_handle);
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
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)_event_data;
    esp_mqtt_client_handle_t client = event->client;
    esp_mqtt_event_id_t event_id = (esp_mqtt_event_id_t)_event_id;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED: {
            m.retry_count = 0;
            clear_msg_pending();
        }
        break;

        case MQTT_EVENT_DISCONNECTED: {
            m.retry_count++;
            if (m.retry_count < MQTT_RETRY_MAX) {
                esp_mqtt_client_reconnect(client);
#ifdef DISPLAY_STATE
                ESP_LOGI(TAG, "Retrying to connect to the MQTT broker");
#endif                
            } else {
                ESP_LOGE(TAG, "Maximum retry attempts reached");
            }
        } break;

        case MQTT_EVENT_SUBSCRIBED: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
#endif                
        } break;

        case MQTT_EVENT_UNSUBSCRIBED: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
#endif                
        } break;

        case MQTT_EVENT_PUBLISHED: {
            if (pop_msg_id(event->msg_id)) {
#ifdef DISPLAY_STATE
                ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED message ID = %d ", event->msg_id);
#endif
            } else {
                ESP_LOGE(TAG, "MQTT_EVENT_PUBLISHED. Error message ID unknown, message ID = %d ", event->msg_id);
            }
        } break;

        case MQTT_EVENT_DATA: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
#endif
        } break;

        case MQTT_EVENT_ERROR: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
#endif
        } break;

        default: {
        } break;

    }
}

size_t Mqtt::make_topic(char* _topic, size_t _length, const char* _device_serial_number, const char* _key) {
    if ((_topic == nullptr) || (_length < 24) || (_device_serial_number == nullptr) || (_key == nullptr)) {
        return (0);
    }
    return (snprintf(_topic, _length - 1, "/%s/%s/%s", SENSOR_ID, _device_serial_number, _key));
}

void Mqtt::perform_publishing(void) {
    if (m.sensor != nullptr) {
        size_t count = m.sensor->get_property_count();
        if (count > 0) {
            SensorReading* reading = m.sensor->get_reading();

            char device_serial_number[32]{ 0 };
            Tools::get_device_serial_number(device_serial_number, sizeof (device_serial_number));

            const SensorProperty* props = m.sensor->get_properties();
            for (int i = 0; i < count; i++) {
                char topic[128]{ 0 };
                if (Mqtt::make_topic(topic, sizeof (topic), device_serial_number, props[i].key) > 0) {
                    float value = 0.0f;
                    if (reading->get_modified_value(props[i].key, value)) {
                        char message[64]{ 0 };
                        SensorProperty::format_value(&props[i], value, message, sizeof (message));
                        int msg_id = esp_mqtt_client_publish(m.client, topic, message, 0, 1, 0);
                        if (push_msg_id(msg_id)) {
#ifdef DISPLAY_STATE
                            ESP_LOGI(TAG, "Sending topic: \"%s\" message: \"%s\", message ID = %d ", topic, message, msg_id);
#endif
                        } else {
                            ESP_LOGE(TAG, "Error sending topic (too many messages pending), topic: \"%s\" message: \"%s\", message ID = %d ", topic, message, msg_id);
                        }
                    }
                }
            }
        }
    }
}

bool Mqtt::push_msg_id(int _msg_id) {
    for (int i = 0; i < MQTT_MSG_MAX; i++) {
        if (m.message_id[i] == 0) {
            m.message_id[i] = _msg_id;
            return (true);
        }
    }
    return (false);
}

bool Mqtt::pop_msg_id(int _msg_id) {
    for (int i = 0; i < MQTT_MSG_MAX; i++) {
        if (m.message_id[i] == _msg_id) {
            m.message_id[i] = 0;
            return (true);
        }
    }
    return (false);
}

int Mqtt::get_msg_pending(void) {
    int count = 0;
    for (int i = 0; i < MQTT_MSG_MAX; i++) {
        count += (int)(m.message_id[i] != 0);
    }
    return (count);
}

void Mqtt::clear_msg_pending(void) {
    memset(m.message_id, 0, sizeof (m.message_id));
}

void Mqtt::_mqtt_task(void* pvParameters) {
    (reinterpret_cast<Mqtt*>(pvParameters))->mqtt_task();
}
void Mqtt::mqtt_task(void) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        if (get_msg_pending() == 0) {
            perform_publishing();
        }
    }
}
