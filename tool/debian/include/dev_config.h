
/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,      Preliminary Configuration Tool
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

#define WCFG_VERSION                 (0x20260710)
// #define WCFG_VERSION                 (0x20260701)
// #define WCFG_VERSION                 (0x20260410)

#define JSON_KEY_VERSION             "version"
#define JSON_KEY_WIFI_SSID           "ssid"
#define JSON_KEY_WIFI_PASSWORD       "password"
#define JSON_KEY_WIFI_CHANNEL        "wifi_channel"
#define JSON_KEY_MQTT_BROKER         "mqtt_broker"
#define JSON_KEY_MQTT_USERNAME       "mqtt_username"
#define JSON_KEY_MQTT_PASSWORD       "mqtt_password"
#define JSON_KEY_MQTT_ENABLE         "mqtt_enable"
#define JSON_KEY_DISPLAY_LAYOUT      "display_layout"
#define JSON_KEY_DISPLAY_PARAM       "display_param"
#define JSON_KEY_DISPLAY_ROTATION    "display_rotation"
#define JSON_KEY_DISPLAY_TIMEOUT     "display_timeout"
#define JSON_KEY_DISPLAY_CONTRAST    "display_contrast"
#define JSON_KEY_SENSOR_TYPE         "sensor_type"
#define JSON_KEY_LED_INTENSITY       "led_intensity"

#define WIFI_AP_NAME_MAX             (32)
#define MQTT_BROKER_MAX              (64)
#define USERNAME_MAX                 (32)
#define PASSWORD_MAX                 (32)
#define NUM_FIELD_LEN                (16)

#define LED_INTENSITY_MAX            (1.00f)
#define LED_INTENSITY_HIGH           (0.75f)
#define LED_INTENSITY_MEDIUM         (0.50f)
#define LED_INTENSITY_LOW            (0.25f)
#define LED_INTENSITY_VERY_LOW       (0.05f)
#define LED_INTENSITY_MIN            (0.01f)
#define LED_INTENSITY_OFF            (0.00f)
#define LED_DEFAULT_INTENSITY        (LED_INTENSITY_VERY_LOW)

#define DISPLAY_LAYOUT_DEFAULT       (0)
#define DISPLAY_PARAM_DEFAULT        (0)
#define DISPLAY_CONTRAST_DEFAULT     (0.80f)
#define DISPLAY_TIMEOUT_DEFAULT      (0.0f)
#define DEFAULT_IFC_ENABLE           (true)
#define DEFAULT_MQTT_ENABLE          (false)
#define DEFAULT_FLAGS                (0x04)
#define WIFI_DEFAULT_CHANNEL         (1)
#define SENSOR_TYPE_DEFAULT          (0)

class KeyList {
    public:
        const char* key;
        char*       field;
        size_t      length;
};

class DevConfig {
    public:
        static const char* config_json_cmd_format;

        char ifac[PATH_MAX]{ 0 };
        
        struct {
            char version[NUM_FIELD_LEN]{ 0 };
            
            char wifi_ssid[WIFI_AP_NAME_MAX]{ 0 };
            char wifi_password[PASSWORD_MAX]{ 0 };
            char wifi_channel[NUM_FIELD_LEN]{ 0 };
            
            char mqtt_broker[MQTT_BROKER_MAX]{ 0 };
            char mqtt_username[USERNAME_MAX]{ 0 };
            char mqtt_password[PASSWORD_MAX]{ 0 };
            char mqtt_enable[NUM_FIELD_LEN]{ 0 };

            char display_timeout_s[NUM_FIELD_LEN]{ 0 };
            char display_contrast[NUM_FIELD_LEN]{ 0 };
            char display_rotoation[NUM_FIELD_LEN]{ 0 };
            char display_layout[NUM_FIELD_LEN]{ 0 };
            char display_param[NUM_FIELD_LEN]{ 0 };
            
            char led_intensity[NUM_FIELD_LEN]{ 0 };
            char sensor_type[NUM_FIELD_LEN]{ 0 };

            char _sensor_type_list[256]{ 0 };
        } cfg;

        struct {
            char identification[8]{ 0 };
            char manufacturer[32]{ 0 };
            char product[32]{ 0 };
            char device_serial_number[22]{ 0 };
            char firmware_version[16]{ 0 };
            char firmware_date[12]{ 0 };
            char head[16]{ 0 };
            char head_serial[16]{ 0 };
            char chip_type[32]{ 0 };
            char wifi_station_mac[20]{ 0 };
            char wifi_ap_mac[20]{ 0 };
            char bluetooth_mac[20]{ 0 };
            char ip_addr[16]{ 0 };
            char rssi[16]{ 0 };
            char tx_power[16]{ 0 };
            char system_time[32]{ 0 };
        } id;

        DevConfig() {
            clear();
        }

        ~DevConfig() {
        }

        static size_t json_get(char* json_data, const char* _key, char* _buffer, size_t _length);
        static void import_data(char* _json_string, KeyList* _key_list, size_t _size);
        
        void clear(void);
        bool parse_id_json(char* _id_json);
        bool parse_config_json(char* _config_json);
        char* get_config_json(const char* _command = nullptr, size_t* _length = nullptr);
};
