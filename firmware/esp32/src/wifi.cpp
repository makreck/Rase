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

esp_err_t Wifi_Station::init(const char *ssid, const char *password) {
    if (wifi_station_init() == ESP_OK) {
        if (wifi_scan() == ESP_OK) {
            if (wifi_connect(ssid, password) == ESP_OK) {
                esp_event_post(APP_EVENT, (int32_t)AppEvent::wifi_enabled, nullptr, 0, pdMS_TO_TICKS(100));
                return (ESP_OK);
            }
        }
    }
    return (ESP_FAIL);
}

esp_err_t Wifi_Station::tear_down(void) {
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_NULL);
    return (ESP_OK);
}

bool Wifi_Station::is_wifi_connected(void) {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        return true;
    }
    return false;
}

esp_err_t Wifi_Station::wifi_set_config(const char* ssid, const char* password) {
    if ((ssid == nullptr) || (password == nullptr)) {
        return (ESP_FAIL);
    }

    memset(&m.wifi_config, 0, sizeof (m.wifi_config));
    m.wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    m.wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
    strncpy((char *)m.wifi_config.sta.ssid, ssid, sizeof(m.wifi_config.sta.ssid));
    strncpy((char *)m.wifi_config.sta.password, password, sizeof(m.wifi_config.sta.password));

    for (int i = 0; i < m.ap_list_count; i++) {
        if (!strcmp((char*)m.wifi_config.sta.ssid, (const char*)m.ap_info[i].ssid)) {
            m.wifi_config.sta.channel = m.ap_info[i].primary;
            return (ESP_OK);
        }
    }

    return (esp_wifi_set_config(WIFI_IF_STA, &m.wifi_config));
}

esp_err_t Wifi_Station::disconnect(void) {
    esp_event_post(APP_EVENT, (int32_t)AppEvent::wifi_disconnected, nullptr, 0, pdMS_TO_TICKS(10));
    return (esp_wifi_disconnect());
}

const char* Wifi_Station::get_ip(void) {
    return (m.ip_v4);
}

const char* Wifi_Station::get_ssid(void) {
    return ((const char*)m.wifi_config.sta.ssid);
}

const char* Wifi_Station::get_password(void) {
    return ((const char*)m.wifi_config.sta.password);
}

int Wifi_Station::get_channel(void) {
    if (m.ap_selected < 0) {
        return (-1);
    }
    return ((int)m.ap_info[m.ap_selected].primary);
}

void Wifi_Station::_wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    (reinterpret_cast<Wifi_Station*>(arg))->wifi_event_handler(event_base, event_id, event_data);
}
void Wifi_Station::wifi_event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (m.s_retry_num < 5) {
            esp_wifi_connect();
            m.s_retry_num++;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        memset(m.ip_v4, 0, sizeof (m.ip_v4));
        snprintf(m.ip_v4, sizeof (m.ip_v4), IPSTR, IP2STR(&event->ip_info.ip));
        m.s_retry_num = 0;
        esp_event_post(APP_EVENT, (int32_t)AppEvent::wifi_connected, m.ip_v4, strlen(m.ip_v4) + 1, pdMS_TO_TICKS(10));
    }
}

esp_err_t Wifi_Station::wifi_station_init(void) {
    memset(&m.wifi_config, 0, sizeof (m.wifi_config));
    m.wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    m.wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    m.s_wifi_event_group = xEventGroupCreate();
    m.s_retry_num = 0;

    esp_netif_create_default_wifi_sta();

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_Station::_wifi_event_handler, this);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi_Station::_wifi_event_handler, this);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t result = esp_wifi_init(&cfg);
    if (result != ESP_OK) return (result);

    result = esp_wifi_set_mode(WIFI_MODE_STA);
    if (result != ESP_OK) return (result);

    result = esp_wifi_set_config(WIFI_IF_STA, &m.wifi_config);
    if (result != ESP_OK) return (result);

    result = esp_wifi_start();
    if (result != ESP_OK) return (result);

    return (esp_wifi_set_max_tx_power(20));
}

esp_err_t Wifi_Station::wifi_scan(void) {
    m.ap_list_count = WIFI_STATION_LIST_MAX;
    esp_wifi_scan_start(NULL, true);
    esp_wifi_scan_get_ap_records(&m.ap_list_count, m.ap_info);
    if (m.ap_list_count == 0) {
        return (ESP_FAIL);
    }

#ifdef DISPLAY_STATE
    print_scan_results();
#endif

    return (ESP_OK);
}

esp_err_t Wifi_Station::wifi_connect(const char* ssid, const char* password) {
    m.ap_selected = -1;
    for (int i = 0; i < m.ap_list_count; i++) {
        if (!strcmp((char*)m.ap_info[i].ssid, ssid)) {
            m.ap_selected = i;
            break;
        }
    }

    if (m.ap_selected < 0) {
        esp_event_post(APP_EVENT, (int32_t)AppEvent::wifi_AP_not_found, nullptr, 0, pdMS_TO_TICKS(10));
        return (ESP_FAIL);
    }

    memset(&m.wifi_config, 0, sizeof (m.wifi_config));
    m.wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    m.wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
    m.wifi_config.sta.channel = m.ap_info[m.ap_selected].primary;
    strcpy((char*)m.wifi_config.sta.ssid, ssid);
    strcpy((char*)m.wifi_config.sta.password, password);
    esp_err_t result = esp_wifi_set_config(WIFI_IF_STA, &m.wifi_config);
    if (result != ESP_OK) return (result);
    return (esp_wifi_connect());
}

void Wifi_Station::print_scan_results(void) {
    ESP_LOGI(TAG, "------------------------");
    ESP_LOGI(TAG, "Found %d APs", m.ap_list_count);
    ESP_LOGI(TAG, "------------------------");
    for (int i = 0; i < m.ap_list_count; i++) {
        ESP_LOGI(TAG, "SSID: %s", m.ap_info[i].ssid);
        ESP_LOGI(TAG, "RSSI: %d", m.ap_info[i].rssi);
        ESP_LOGI(TAG, "Channel: %d", m.ap_info[i].primary);
        ESP_LOGI(TAG, "Auth Mode: %d", m.ap_info[i].authmode);
        ESP_LOGI(TAG, "------------------------");
    }
}

size_t Wifi_Station::get_rssi_dbm(char* _buffer, size_t _size) {
    if ((_buffer == nullptr) || (_size < 8)) {
        return (0);
    }
    size_t length = 0;
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        length = snprintf(_buffer, _size, "%+.0f dBm", (float)ap_info.rssi);
    } else {
        length = snprintf(_buffer, _size, "--- dBm");
    }
    return (length);
}

size_t Wifi_Station::get_tx_power_dbm(char* _buffer, size_t _size) {
    if ((_buffer == nullptr) || (_size < 8)) {
        return (0);
    }
    size_t length = 0;
    int8_t tx_power_max = 0;
    if (esp_wifi_get_max_tx_power(&tx_power_max) == ESP_OK) {
        float power_dbm = 0.25f * (float)tx_power_max;
        length = snprintf(_buffer, _size, "%+.0f dBm", power_dbm);
    } else {
        length = snprintf(_buffer, _size, "--- dBm");
    }
    return (length);
}
