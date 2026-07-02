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

#pragma once

class Mqtt {
    private:
        struct {
            char broker_uri[64]{ 0 };
            char username[32]{ 0 };
            char password[32]{ 0 };
            esp_mqtt_client_config_t mqtt_cfg;
            esp_mqtt_client_handle_t client = nullptr;
        } m;

        void init(const char* broker_url, const char* _username, const char* _password);
        void cleanup(void);

    public:
        Mqtt(const char* _broker_url, const char* _username, const char* _password) {
            init(_broker_url, _username, _password);
        }

        ~Mqtt() {
            cleanup();
        }

        static void _mqtt_event_handler(void *_handler_args, esp_event_base_t _base, int32_t _event_id, void* _event_data);
        void mqtt_event_handler(esp_event_base_t _base, int32_t _event_id, void* _event_data);

        void start(void);
        void stop(void);

};
