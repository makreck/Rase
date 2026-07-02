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

#define WCFG_MAGIC               (0x20260701)
// #define WCFG_MAGIC               (0x20260410)

#define WCFG_STORAGE_NAMESPACE   SENSOR_ID
#define WCFG_STORAGE_CONFIG_KEY  "system-config"

#define LED_INTENSITY_MAX        (1.00f)
#define LED_INTENSITY_HIGH       (0.75f)
#define LED_INTENSITY_MEDIUM     (0.50f)
#define LED_INTENSITY_LOW        (0.25f)
#define LED_INTENSITY_VERY_LOW   (0.05f)
#define LED_INTENSITY_MIN        (0.01f)
#define LED_INTENSITY_OFF        (0.00f)
#define LED_DEFAULT_INTENSITY    (LED_INTENSITY_VERY_LOW)

#define DISPLAY_CONTRAST_HIGH    (1.0f)
#define DISPLAY_CONTRAST_MEDIUM  (0.5f)
#define DISPLAY_CONTRAST_LOW     (0.0f)
#define DISPLAY_CONTRAST_DEFAULT (DISPLAY_CONTRAST_HIGH)

#ifdef ESP32_S3_WROOM_1
    #define DISPLAY_ROTATION_DEFAULT (1)
#else
    #define DISPLAY_ROTATION_DEFAULT (0)
#endif

#define CONFIG_IFC_ENABLED       (0x01)
#define CONFIG_IFC_DISABLED      (0x00)

#define WIFI_DEFAULT_CHANNEL     (1)
#define WIFI_MAX_CONNECTION      (10)

// *** Misc default data, later to be defined in their modules:
#define T_APP_DISPLAY_TIMEOUT_S  (0.0f)
#define DISPLAY_LAYOUT_DEFAULT   ((uint8_t)(DisplayLayout::large_values))
#define SENSOR_TYPE_DEFAULT      ((uint8_t)SensorType::autoscan)

#define WIFI_AP_NAME_MAX         (32)
#define WIFI_AP_PASSWD_MAX       (64)
#define MQTT_BROKER_MAX          (64)
#define MQTT_USERNAME_MAX        (32)
#define MQTT_PASSWORD_MAX        (32)

enum class DisplayLayout {
    large_values = 0,
    detailes     = 1,
    info         = 2,
};

class SysConfigData {
    public:
        union {
            struct {
                uint32_t magic_id;
                char ssid[WIFI_AP_NAME_MAX];
                char password[WIFI_AP_PASSWD_MAX];
                char mqtt_broker[MQTT_BROKER_MAX];
                char mqtt_username[MQTT_USERNAME_MAX];
                char mqtt_password[MQTT_PASSWORD_MAX];
                uint8_t channel;
                uint8_t display_layout;
                uint8_t display_param;
                uint8_t display_rotation;
                uint8_t reserved1[2];
                uint8_t config_interface_enable;
                uint8_t sensor_type;
                float display_timeout_s;
                float led_intensity;
                float display_contrast;
            };
            uint8_t data[256];
        };
};

class SysConfig {
    private:
        SysConfigData cfg;
        bool modified = false;

    public:
        SysConfig(void) {
            init();
        }
        
        ~SysConfig() {
            cleanup();
        }

        void print_parms(const char* hint = nullptr);

        uint8_t get_channel(void) { return (cfg.channel); }

        static AppState get_mac_Address(char* string, size_t size);
        
        AppState init(void);
        AppState cleanup(void);
        AppState load_defaults(void);
        AppState init_nvs_flash(bool forceInit = false);
        AppState check(void);
        AppState load(void);
        AppState save(void);
        AppState update(void);
        AppState perform_factory_reset(void);

        AppState set_wifi_channel(uint8_t channelNumber);
        AppState set_display_timeout(float timeout_s);
        AppState set_LED_intensity(float intensity);
        AppState set_display_contrast(float value);
        AppState set_display_layout(DisplayLayout layout);
        AppState set_display_parameter(uint8_t parameter = 0);
        AppState set_ssid(const char* ap_name);
        AppState set_password(const char* password);
        AppState set_mqtt_broker(const char* broker);
        AppState set_mqtt_username(const char* username);
        AppState set_mqtt_password(const char* password);
        AppState set_sensor_type(SensorType type);
        
        const char* get_ssid(void);
        const char* get_password(void);
        const char* get_mqtt_broker(void);
        const char* get_mqtt_username(void);
        const char* get_mqtt_password(void);
        
        float get_display_timeout(void);
        float get_LED_intensity(void);
        float get_display_contrast(void);

        DisplayLayout get_display_layout(void);
        uint8_t get_display_parameter(void);
        int get_wifi_channel(void);
        uint8_t get_rotation(void);
        SensorType get_sensor_type(void);
        uint8_t get_config_enable(void);
        AppState set_config_enable(uint8_t enable);
        AppState flip_Rotation(void);
};
