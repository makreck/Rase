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

#define WIFI_STATION_LIST_MAX (20)

class Wifi_Station {
    private:
        struct {
            wifi_config_t wifi_config;

            EventGroupHandle_t s_wifi_event_group = nullptr;
            // esp_event_handler_instance_t instance_any_id = nullptr;
            // esp_event_handler_instance_t instance_got_ip = nullptr;
            int s_retry_num = 0;

            wifi_ap_record_t ap_info[WIFI_STATION_LIST_MAX];
            uint16_t ap_list_count = 0;
            int16_t ap_selected = -1;

            char ip_v4[16]{0};
        } m;

        static void _wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        void wifi_event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data);
        
        esp_err_t init(const char* ssid, const char* password);
        esp_err_t tear_down(void);

        esp_err_t wifi_set_config(const char* ssid, const char* password);

        void print_scan_results(void);

        esp_err_t wifi_station_init(void);
        esp_err_t wifi_scan(void);
        esp_err_t wifi_connect(const char* ssid, const char* password);

    public:
        Wifi_Station(const char* ssid, const char* password) {
            init(ssid, password);
        }

        ~Wifi_Station() {
            tear_down();
        }

        static size_t get_rssi_dbm(char* _buffer, size_t _size);
        static size_t get_tx_power_dbm(char* _buffer, size_t _size);

        esp_err_t disconnect(void);

        bool is_wifi_connected(void);

        const char* get_ip(void);
        const char* get_ssid(void);
        const char* get_password(void);
        int get_channel(void);
};

