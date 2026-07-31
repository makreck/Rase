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

#define WCFG_VERSION                 (0x20260710)
// #define WCFG_VERSION                 (0x20260701)
// #define WCFG_VERSION                 (0x20260410)

#define WCFG_STORAGE_NAMESPACE       SENSOR_ID
#define WCFG_STORAGE_CONFIG_KEY      "system-config"

#define LED_INTENSITY_MAX            (1.00f)
#define LED_INTENSITY_HIGH           (0.75f)
#define LED_INTENSITY_MEDIUM         (0.50f)
#define LED_INTENSITY_LOW            (0.25f)
#define LED_INTENSITY_VERY_LOW       (0.05f)
#define LED_INTENSITY_MIN            (0.01f)
#define LED_INTENSITY_OFF            (0.00f)
#define LED_DEFAULT_INTENSITY        (LED_INTENSITY_VERY_LOW)

#define DISPLAY_CONTRAST_DEFAULT     (0.80f)

#define DEFAULT_IFC_ENABLE           (true)
#define DEFAULT_MQTT_ENABLE          (false)

#ifdef ESP32_S3_WROOM_1
    #define DISPLAY_ROTATION_DEFAULT (180)
#else
    #define DISPLAY_ROTATION_DEFAULT (0)
#endif

#define WIFI_DEFAULT_CHANNEL         (1)
#define WIFI_MAX_CONNECTION          (10)

// *** Misc default data, later to be defined in their modules:
#define T_APP_DISPLAY_TIMEOUT_S      (0.0f)
#define DISPLAY_LAYOUT_DEFAULT       (DisplayLayout::large_values)
#define SENSOR_TYPE_DEFAULT          (SensorType::autoscan)

#define WIFI_AP_NAME_MAX             (32)
#define WIFI_AP_PASSWD_MAX           (64)
#define MQTT_BROKER_MAX              (64)
#define MQTT_USERNAME_MAX            (32)
#define MQTT_PASSWORD_MAX            (32)

enum class DisplayLayout {
    large_values = 0,
    detailes     = 1,
    info         = 2,
};

class SysConfigData {
    public:
        union {
            struct {
                uint32_t version;
                char ssid[WIFI_AP_NAME_MAX];
                char password[WIFI_AP_PASSWD_MAX];
                char mqtt_broker[MQTT_BROKER_MAX];
                char mqtt_username[MQTT_USERNAME_MAX];
                char mqtt_password[MQTT_PASSWORD_MAX];
                uint8_t wifi_channel;
                uint8_t sensor_type;
                uint8_t display_layout;
                uint8_t display_param;
                uint8_t reserved1[3];
                union {
                    uint8_t flags;
                    struct {
                        uint8_t f_display_rotoation : 2;
                        uint8_t f_ifc_enable        : 1;
                        uint8_t f_mqtt_enable       : 1;
                        uint8_t f_reserved          : 4;
                    };
                };

                float display_timeout_s;
                float led_intensity;
                float display_contrast;
            };
            uint8_t data[256];
        };
};

class SysConfig;

class JsonScan {
    public:
        const char* key;
        AppState (*scan_function)(SysConfig*, const char *);
};

class SysConfig {
    private:
        static const char* config_json_format;
        static const char* str_display_layout[3];
        static const JsonScan config_scan_table[];

        SysConfigData cfg;
        bool modified = false;
        char format[32]{ 0 };

    public:
        SysConfig(void) {
            init();
        }
        
        ~SysConfig() {
            cleanup();
        }

        void print_parms(const char* hint = nullptr);

        static AppState get_mac_Address(char* string, size_t size);
        static const char* get_display_layout_name(DisplayLayout __layout);
        
        AppState init(void);
        AppState cleanup(void);
        AppState load_defaults(void);
        AppState init_nvs_flash(bool forceInit = false);
        AppState check(void);
        AppState load(void);
        AppState save(void);
        AppState update(void);
        AppState perform_factory_reset(void);

        AppState set_wifi_channel(int _channelNumber);
        AppState set_display_timeout(float _timeout_s);
        AppState set_display_rotation(int _degrees);
        AppState set_display_contrast(float _value);
        AppState set_display_layout(DisplayLayout layout);
        AppState set_display_parameter(int _parameter = 0);
        AppState set_ssid(const char* _ap_name);
        AppState set_password(const char* _password);
        AppState set_mqtt_broker(const char* _broker);
        AppState set_mqtt_username(const char* _username);
        AppState set_mqtt_password(const char* _password);
        AppState set_mqtt_enable(bool enable);
        AppState set_sensor_type(SensorType _type);
        AppState set_LED_intensity(float _intensity);

        static AppState set_wifi_ssid_str(SysConfig* _instance, const char* _ap_name);
        static AppState set_wifi_password_str(SysConfig* _instance, const char* _password);
        static AppState set_wifi_channel_str(SysConfig* _instance, const char* _channel);

        static AppState set_mqtt_broker_str(SysConfig* _instance, const char* _broker);
        static AppState set_mqtt_username_str(SysConfig* _instance, const char* _username);
        static AppState set_mqtt_password_str(SysConfig* _instance, const char* _password);

        static AppState set_display_rotation_str(SysConfig* _instance, const char* _degrees);
        static AppState set_display_contrast_str(SysConfig* _instance, const char* _value);
        static AppState set_display_timeout_str(SysConfig* _instance, const char* _str);
        static AppState set_display_layout_str(SysConfig* _instance, const char* _layout);
        static AppState set_display_parameter_str(SysConfig* _instance, const char* _parameter);
        static AppState set_mqtt_enable_str(SysConfig* _instance, const char* _enable);
        static AppState set_sensor_type_str(SysConfig* _instance, const char* _type);
        static AppState set_LED_intensity_str(SysConfig* _instance, const char* _intensity);
        
        const char* get_ssid(void);
        const char* get_password(void);
        const char* get_mqtt_broker(void);
        const char* get_mqtt_username(void);
        const char* get_mqtt_password(void);
        const char* get_display_timeout_str(void);
        const char* get_display_layout_str(void);
                
        float get_display_timeout(void);
        float get_LED_intensity(void);
        float get_display_contrast(void);
        AppState import_json(const char* _json_string, size_t _length);
        char* get_json(bool _hide_passwords);
        DisplayLayout get_display_layout(void);
        uint8_t get_display_parameter(void);
        int get_wifi_channel(void);
        int get_display_rotation(void);
        AppState flip_display_rotation(void);
        SensorType get_sensor_type(void);
        bool get_config_enable(void);
        AppState set_config_enable(bool enable);
        bool get_mqtt_enable(void);
};
