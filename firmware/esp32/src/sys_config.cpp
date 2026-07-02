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

#define DISPLAY_STATE

AppState SysConfig::init(void) {
    init_nvs_flash();
    load_defaults();
    AppState result = load();
    if (result != AppState::OK) {
        result = perform_factory_reset();
#ifdef DISPLAY_STATE    
    } else {
        print_parms("System parameters loaded");
#endif
    }
    return (result);
}

AppState SysConfig::cleanup(void) {
    return (update());
}

AppState SysConfig::perform_factory_reset(void) {
    init_nvs_flash(true);
    load_defaults();
    AppState result = save();
#ifdef DISPLAY_STATE    
    print_parms("System parameters initialized");
#endif
    return (result);
}

AppState SysConfig::init_nvs_flash(bool forceInit) {
    esp_err_t status = nvs_flash_init();
    if (forceInit || (status == ESP_ERR_NVS_NO_FREE_PAGES) || (status == ESP_ERR_NVS_NEW_VERSION_FOUND)) {
        status = nvs_flash_erase();
        if (status != ESP_OK) return (AppState::nvs_error);
        status = nvs_flash_init();
        if (status != ESP_OK) return (AppState::nvs_error);
    }
    return (AppState::OK);
}

AppState SysConfig::load_defaults(void) {
    memset(&cfg, 0, sizeof (cfg));
    cfg.magic_id                = WCFG_MAGIC;
    cfg.channel                 = WIFI_DEFAULT_CHANNEL;
    cfg.display_layout          = DISPLAY_LAYOUT_DEFAULT;
    cfg.display_param           = 0;
    cfg.display_rotation        = DISPLAY_ROTATION_DEFAULT;
    cfg.display_timeout_s       = T_APP_DISPLAY_TIMEOUT_S;
    cfg.display_contrast        = DISPLAY_CONTRAST_DEFAULT;
    cfg.config_interface_enable = CONFIG_IFC_ENABLED;
    cfg.sensor_type             = SENSOR_TYPE_DEFAULT;
    cfg.led_intensity           = LED_DEFAULT_INTENSITY;
    return (AppState::OK);
}

AppState SysConfig::check(void) {
    if (cfg.magic_id != WCFG_MAGIC) return (AppState::access_denied);
    if ((cfg.channel < 1) || (cfg.channel > 12)) return (AppState::invalid_arg);
    return (AppState::OK);
}

AppState SysConfig::load(void) {
    nvs_handle_t handle = 0;
    esp_err_t result = nvs_open(WCFG_STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    if (result != ESP_OK) {
        return (AppState::nvs_error);
    }
    size_t length = sizeof(cfg);
    result = nvs_get_blob(handle, WCFG_STORAGE_CONFIG_KEY, &cfg, &length);
    nvs_close(handle);
    if (length != sizeof(cfg)) {
        return (AppState::nvs_error);
    }
    return (check());
}

AppState SysConfig::save(void) {
    if (cfg.magic_id != WCFG_MAGIC) {
        return (AppState::access_denied);
    }
    nvs_handle_t handle = 0;
    esp_err_t result = nvs_open(WCFG_STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    if (result != ESP_OK) {
        return (AppState::nvs_error);
    }
    result = nvs_set_blob(handle, WCFG_STORAGE_CONFIG_KEY, &cfg, sizeof(cfg));
    nvs_close(handle);

#ifdef DISPLAY_STATE    
    print_parms("Saved system parameters");
#endif

    return ((result == ESP_OK) ? AppState::OK : AppState::nvs_error);
}

AppState SysConfig::update(void) {
    AppState result = AppState::OK;
    if (modified) {
        result = save();
        if (result == AppState::OK) {
            modified = false;
        }
    }
    return (result);
}

float SysConfig::get_display_timeout(void) {
    return (cfg.display_timeout_s);
}

AppState SysConfig::set_display_timeout(float timeout_s) {
    if ((cfg.display_timeout_s != timeout_s) && (timeout_s >= 0.0f)) {
        cfg.display_timeout_s = timeout_s;
        modified = true;
    }
    return (AppState::OK);
}

AppState SysConfig::set_wifi_channel(uint8_t channelNumber) {
    if ((channelNumber < 1) || (channelNumber > 12)) {
        return (AppState::invalid_arg);
    }
    modified = (cfg.channel != channelNumber);
    cfg.channel = channelNumber;
    return (AppState::OK);
}

int SysConfig::get_wifi_channel(void) {
    if (cfg.magic_id != WCFG_MAGIC) {
        return (-1);
    }
    return (cfg.channel); 
}

const char* SysConfig::get_ssid(void) { 
    if (cfg.magic_id != WCFG_MAGIC) {
        return (nullptr);
    }
    return (cfg.ssid); 
}

AppState SysConfig::set_ssid(const char* ap_name) {
    if (ap_name == nullptr) {
        return (AppState::invalid_arg);
    }
    if (!strncmp(cfg.ssid, ap_name, sizeof (cfg.ssid))) {
        return (AppState::OK);
    }
    memset(cfg.ssid, 0, sizeof(cfg.ssid));
    strncpy(cfg.ssid, ap_name, sizeof(cfg.ssid) - 1);
    modified = true;
    return (AppState::OK);
}

const char* SysConfig::get_password(void) {
    if (cfg.magic_id != WCFG_MAGIC) {
        return (nullptr);
    }
    return (cfg.password); 
}

AppState SysConfig::set_password(const char* password) {
    if ((password != nullptr) && (!strncmp(cfg.password, password, sizeof (cfg.password)))) {
        return (AppState::OK);
    }
    memset(cfg.password, 0, sizeof (cfg.password));
    if (password != nullptr) {
        strncpy(cfg.password, password, sizeof (cfg.password) - 1);
    }
    modified = true;
    return (AppState::OK);
}

AppState SysConfig::set_LED_intensity(float intensity) {
    float preset = MAX(0.0f, MIN(1.0f, intensity));
    modified = (cfg.led_intensity != preset);
    cfg.led_intensity = preset;
    return (AppState::OK);
}

float SysConfig::get_LED_intensity(void) {
    return (cfg.led_intensity);
}

uint8_t SysConfig::get_rotation(void) {
    return (cfg.display_rotation);
}

SensorType SysConfig::get_sensor_type(void) {
    return ((SensorType)cfg.sensor_type);
}

uint8_t SysConfig::get_config_enable(void) {
    return (cfg.config_interface_enable);
}

AppState SysConfig::set_config_enable(uint8_t enable) {
    modified = (cfg.config_interface_enable != enable);
    cfg.config_interface_enable = enable;
    return (AppState::OK);
}

AppState SysConfig::set_sensor_type(SensorType type) {
    cfg.sensor_type = (uint8_t)type;
    modified = true;
    return (AppState::OK);
}

AppState SysConfig::flip_Rotation(void) {
    cfg.display_rotation ^= 0x01;
    modified = true;
    return (AppState::OK);
}

AppState SysConfig::set_display_contrast(float value) {
    float contrast = MIN(1.0f, MAX(0.0f, value));
    if (cfg.display_contrast != contrast) {
        cfg.display_contrast = contrast;
        modified = true;
    }
    return (AppState::OK);
}

float SysConfig::get_display_contrast(void) {
    return (cfg.display_contrast);
}

AppState SysConfig::get_mac_Address(char* string, size_t size) {
    uint8_t mac_byte_buffer[32]{ 0 };
    esp_err_t result = esp_efuse_mac_get_default(mac_byte_buffer);
    if (result != ESP_OK) {
        return (AppState::failed);
    }
    snprintf(string,
        (size / sizeof(string[0])) - 1,
        "%02X%02X%02X%02X%02X%02X",
        mac_byte_buffer[0],
        mac_byte_buffer[1],
        mac_byte_buffer[2],
        mac_byte_buffer[3],
        mac_byte_buffer[4],
        mac_byte_buffer[5]);
    return (AppState::OK);
}

AppState SysConfig::set_display_layout(DisplayLayout layout) {
    if (cfg.display_layout != (uint8_t)layout) {
        cfg.display_layout  = (uint8_t)layout;
        cfg.display_param   = (uint8_t)0;
        modified = true;
    }
    return (AppState::OK);
}

DisplayLayout SysConfig::get_display_layout(void) {
    return ((DisplayLayout)cfg.display_layout);
}

AppState SysConfig::set_display_parameter(uint8_t parameter) {
    if (cfg.display_param != parameter) {
        modified = true;
        cfg.display_param = parameter;
    }
    return (AppState::OK);
}

uint8_t SysConfig::get_display_parameter(void) {
    return (cfg.display_param);
}

AppState SysConfig::set_mqtt_broker(const char* broker) {
    if (broker == nullptr) {
        return (AppState::invalid_arg);
    }
    if (!strncmp(cfg.mqtt_broker, broker, sizeof (cfg.mqtt_broker))) {
        return (AppState::OK);
    }
    memset(cfg.mqtt_broker, 0, sizeof(cfg.mqtt_broker));
    strncpy(cfg.mqtt_broker, broker, sizeof(cfg.mqtt_broker) - 1);
    modified = true;
    return (AppState::OK);
}

const char* SysConfig::get_mqtt_broker(void) {
    return (cfg.mqtt_broker);
}

AppState SysConfig::set_mqtt_username(const char* username) {
    if ((username != nullptr) && (!strncmp(cfg.mqtt_username, username, sizeof (cfg.mqtt_username)))) {
        return (AppState::OK);
    }
    memset(cfg.mqtt_username, 0, sizeof (cfg.mqtt_username));
    if (username != nullptr) {
        strncpy(cfg.mqtt_username, username, sizeof (cfg.mqtt_username) - 1);
    }
    modified = true;
    return (AppState::OK);
}

const char* SysConfig::get_mqtt_username(void) {
    return (cfg.mqtt_username);
}

AppState SysConfig::set_mqtt_password(const char* password) {
    if ((password != nullptr) && (!strncmp(cfg.mqtt_password, password, sizeof (cfg.mqtt_password)))) {
        return (AppState::OK);
    }
    memset(cfg.mqtt_password, 0, sizeof (cfg.mqtt_password));
    if (password != nullptr) {
        strncpy(cfg.mqtt_password, password, sizeof (cfg.mqtt_password) - 1);
    }
    modified = true;
    return (AppState::OK);
}

const char* SysConfig::get_mqtt_password(void) {
    return (cfg.mqtt_password);
}

void SysConfig::print_parms(const char* hint) {
#ifdef DISPLAY_STATE    
    if (hint == nullptr) {
        hint = "Stored system config parameters";
    }
    ESP_LOGI(TAG, "%s: ", hint);
    ESP_LOGI(TAG, "\t- Size:               %d bytes ", (int)sizeof (cfg));
    ESP_LOGI(TAG, "\t- Magic ID:           0x%-8.8X ", (unsigned int)cfg.magic_id);

    ESP_LOGI(TAG, "\t- AP SSID:            <%s> ",     get_ssid());
    ESP_LOGI(TAG, "\t- AP password:        <%s> ",     get_password());
    ESP_LOGI(TAG, "\t- AP channel:         %d ",       get_wifi_channel());

    ESP_LOGI(TAG, "\t- MQTT broker:        <%s> ",     get_mqtt_broker());
    ESP_LOGI(TAG, "\t- MQTT username:      <%s> ",     get_mqtt_username());
    ESP_LOGI(TAG, "\t- MQTT password:      <%s> ",     get_mqtt_password());

    ESP_LOGI(TAG, "\t- Display Layout:     %d ",       (int)get_display_layout());
    ESP_LOGI(TAG, "\t- Display parameter:  %d ",       (int)get_display_parameter());
    ESP_LOGI(TAG, "\t- Display rotation:   %d   ",     (int)get_rotation());
    ESP_LOGI(TAG, "\t- Display timeout:    %.3f s ",   get_display_timeout());
    ESP_LOGI(TAG, "\t- Display contrast:   %.1f ",     get_display_contrast());

    ESP_LOGI(TAG, "\t- Config interface:   %s ",       ((get_config_enable() & CONFIG_IFC_ENABLED) == CONFIG_IFC_ENABLED) ? "enabled" : "disabled");
    ESP_LOGI(TAG, "\t- Sensor type ID:     %d ",       (int)get_sensor_type());

    ESP_LOGI(TAG, "\t- LED intensity:      %.2f ",     get_LED_intensity());
#endif    
}
