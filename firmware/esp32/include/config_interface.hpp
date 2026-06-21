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

#define RX_BUFFER_SIZE  (1024)
#define TX_BUFFER_SIZE  (1024)

#define CFG_KEY_WEBSITE_RESPONSE "/root"
#define CFG_KEY_SENSOR_RESPONSE  "/api/sensors"
#define CFG_KEY_ID_RESPONSE      "/api/id"
#define CFG_KEY_WIFI_SETUP       "/setup-wifi"
#define CFG_KEY_CONFIG           "/config"

class App;

class ConfigInterface {
    private:
        App* app = nullptr;

        TaskHandle_t task_handle = nullptr;

        char rx_buffer[RX_BUFFER_SIZE];
        char tx_buffer[TX_BUFFER_SIZE];

        static void _communication_handler(void *pvParameters);
        void communication_handler(void);

        static ssize_t send(const char* data, size_t length = 0);

        void init(void);
        void cleanup(void);
        void process_command(const char* data, size_t length);
        void setup(void);
        void handle_wifi_setup(const char* data, size_t length);
        void handle_id_response(void);
        void handle_sensor_response(void);
        void handle_website_response(void);
        void handle_config_response(const char* data, size_t length);

    public:
        ConfigInterface(App* _app) {
            app = _app;
            init();
        }

        ~ConfigInterface() {
            cleanup();
        }
};
