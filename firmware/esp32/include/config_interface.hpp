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

class App;
class ConfigInterface;

typedef void (* Config_Function_t)(ConfigInterface* instance, int mode, const char* data, size_t length);

class ConfigCB {
    public:
        const char* key;
        Config_Function_t callback;
        int mode;
};

class ConfigInterface {
    private:
        App* app = nullptr;

        TaskHandle_t task_handle = nullptr;

        char rx_buffer[RX_BUFFER_SIZE];
        char tx_buffer[TX_BUFFER_SIZE];

        static void extract_link_pwd(const char* _data, size_t _length, const char* _key,
            char* _link, size_t _len_link, char* _username, size_t _len_username, char* _password, size_t _len_password);

        static void _communication_handler(void *pvParameters);
        void communication_handler(void);

        static ssize_t send(const char* data, size_t length = 0);

        void init(void);
        void cleanup(void);
        void process_command(const char* data, size_t length);
        void setup(void);
        
        static void handle_wifi_setup(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);
        static void handle_config_response(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);
        static void handle_id_response(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);
        static void handle_sensor_response(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);
        static void handle_website_output(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);
        static void handle_restart(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);
        static void handle_mqtt_broker(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);
        static void handle_par(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);
        static void handle_test(ConfigInterface* instance, int mode = 0, const char* data = nullptr, size_t length = 0);

        const ConfigCB function_tab[10] = {
            { TTY_KEY_WIFI_SETUP,     ConfigInterface::handle_wifi_setup,      0},
            { TTY_KEY_MQTT_BROKER,    ConfigInterface::handle_mqtt_broker,     0},
            { TTY_KEY_API_CONFIG,     ConfigInterface::handle_config_response, 0},
            { TTY_KEY_API_IDENTIFY,   ConfigInterface::handle_id_response,     0},
            { TTY_KEY_API_SENSORS,    ConfigInterface::handle_sensor_response, 0},
            { TTY_KEY_WEBSITE,        ConfigInterface::handle_website_output,  0},
            { TTY_KEY_API_REBOOT,     ConfigInterface::handle_restart,         0},
            { TTY_KEY_API_INITIALIZE, ConfigInterface::handle_restart,         9},
            { TTY_KEY_PAR,            ConfigInterface::handle_par,             0},

            { TTY_KEY_TEST,           ConfigInterface::handle_test,            0},
        };

    public:
        ConfigInterface(App* _app) {
            app = _app;
            init();
        }

        ~ConfigInterface() {
            cleanup();
        }
};
