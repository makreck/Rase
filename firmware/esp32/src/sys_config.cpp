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

#define JSON_KEY_VERSION          "version"
#define JSON_KEY_WIFI_SSID        "ssid"
#define JSON_KEY_WIFI_PASSWORD    "password"
#define JSON_KEY_WIFI_CHANNEL     "wifi_channel"
#define JSON_KEY_MQTT_BROKER      "mqtt_broker"
#define JSON_KEY_MQTT_USERNAME    "mqtt_username"
#define JSON_KEY_MQTT_PASSWORD    "mqtt_password"
#define JSON_KEY_MQTT_ENABLE      "mqtt_enable"
#define JSON_KEY_DISPLAY_LAYOUT   "display_layout"
#define JSON_KEY_DISPLAY_PARAM    "display_param"
#define JSON_KEY_DISPLAY_ROTATION "display_rotation"
#define JSON_KEY_DISPLAY_TIMEOUT  "display_timeout"
#define JSON_KEY_DISPLAY_CONTRAST "display_contrast"
#define JSON_KEY_SENSOR_TYPE      "sensor_type"
#define JSON_KEY_LED_INTENSITY    "led_intensity"

const char* SysConfig::str_display_layout[3] = {
    "Value page",
    "Details page",
    "Info page",
};

const char* SysConfig::config_json_format =
    "\n"
    "{\n"
    "\t\"" JSON_KEY_VERSION "\": \"0x%-8.8X\",\n"
    "\t\"" JSON_KEY_WIFI_SSID "\": \"%s\",\n"
    "\t\"" JSON_KEY_WIFI_PASSWORD "\": \"%s\",\n"
    "\t\"" JSON_KEY_WIFI_CHANNEL "\": \"%d\",\n"
    "\t\"" JSON_KEY_MQTT_BROKER "\": \"%s\",\n"
    "\t\"" JSON_KEY_MQTT_USERNAME "\": \"%s\",\n"
    "\t\"" JSON_KEY_MQTT_PASSWORD "\": \"%s\",\n"
    "\t\"" JSON_KEY_MQTT_ENABLE "\": \"%s\",\n"
    "\t\"" JSON_KEY_DISPLAY_LAYOUT "\": \"%s\",\n"
    "\t\"" JSON_KEY_DISPLAY_PARAM "\": \"%d\",\n"
    "\t\"" JSON_KEY_DISPLAY_ROTATION "\": \"%d°\",\n"
    "\t\"" JSON_KEY_DISPLAY_TIMEOUT "\": \"%s\",\n"
    "\t\"" JSON_KEY_DISPLAY_CONTRAST "\": \"%.0f%%\",\n"
    "\t\"" JSON_KEY_SENSOR_TYPE "\": \"%s\",\n"
    "\t\"" JSON_KEY_LED_INTENSITY "\": \"%.0f%%\"\n"
    "}\n";

const JsonScan SysConfig::config_scan_table[] = {
    { JSON_KEY_WIFI_SSID,        SysConfig::set_wifi_ssid_str         },
    { JSON_KEY_WIFI_PASSWORD,    SysConfig::set_wifi_password_str     },
    { JSON_KEY_WIFI_CHANNEL,     SysConfig::set_wifi_channel_str      },
    { JSON_KEY_MQTT_BROKER,      SysConfig::set_mqtt_broker_str       },
    { JSON_KEY_MQTT_USERNAME,    SysConfig::set_mqtt_username_str     },
    { JSON_KEY_MQTT_PASSWORD,    SysConfig::set_mqtt_password_str     },
    { JSON_KEY_MQTT_ENABLE,      SysConfig::set_mqtt_enable_str       },
    { JSON_KEY_DISPLAY_LAYOUT,   SysConfig::set_display_layout_str    },
    { JSON_KEY_DISPLAY_PARAM,    SysConfig::set_display_parameter_str },
    { JSON_KEY_DISPLAY_ROTATION, SysConfig::set_display_rotation_str  },
    { JSON_KEY_DISPLAY_TIMEOUT,  SysConfig::set_display_timeout_str   },
    { JSON_KEY_DISPLAY_CONTRAST, SysConfig::set_display_contrast_str  },
    { JSON_KEY_SENSOR_TYPE,      SysConfig::set_sensor_type_str       },
    { JSON_KEY_LED_INTENSITY,    SysConfig::set_LED_intensity_str     },
};

char* SysConfig::get_json(void) {

    char sensor_support[256]{ 0 };
    strncpy(sensor_support, SensorDriver::get_driver_name(get_sensor_type()), sizeof (sensor_support));
    for (size_t i = 0; i < SIZEOFARRAY(SensorDriver::str_sensor_types); i++) {
        strcat(sensor_support, ",");
        strcat(sensor_support, SensorDriver::str_sensor_types[i]);
    }

    size_t length = snprintf(nullptr, 0,
        config_json_format,
        (unsigned int)cfg.version,
        get_ssid(),
        get_password(),
        get_wifi_channel(),
        get_mqtt_broker(),
        get_mqtt_username(),
        get_mqtt_password(),
        (get_mqtt_enable()) ? "enabled" : "disabled",
        get_display_layout_str(),
        (int)get_display_parameter(),
        (int)get_display_rotation(),
        get_display_timeout_str(),
        (float)get_display_contrast() * 100.0f,
        sensor_support,
        (float)(get_LED_intensity() * 100.0f)
    );

    size_t size = length + 8;
    char* str_json = (char*)malloc(size);
    if (str_json != nullptr) {
        memset(str_json, 0, size);
        snprintf(str_json, length + 1,
            config_json_format,
            (unsigned int)cfg.version,
            get_ssid(),
            get_password(),
            get_wifi_channel(),
            get_mqtt_broker(),
            get_mqtt_username(),
            get_mqtt_password(),
            (get_mqtt_enable()) ? "enabled" : "disabled",
            get_display_layout_str(),
            (int)get_display_parameter(),
            (int)get_display_rotation(),
            get_display_timeout_str(),
            (float)get_display_contrast() * 100.0f,
            sensor_support,
            (float)(get_LED_intensity() * 100.0f)
        );
    }

    return (str_json);
}

AppState SysConfig::import_json(const char* _json_string, size_t _length) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "SysConfig::import_json()\n\"\"\"\n%s\n\"\"\"\n", _json_string);
#endif
    if ((_json_string == nullptr) || (_length < 2)) {
        return (AppState::invalid_arg);
    }

    for (int i = 0; i < SIZEOFARRAY(SysConfig::config_scan_table); i++) {
        char parameter[64]{ 0 };
        size_t len = Tools::json_get(_json_string, SysConfig::config_scan_table[i].key, parameter, sizeof (parameter));
        if (len > 0) {
            (*config_scan_table[i].scan_function)(this, parameter);
        }
    }

    return (update());
}

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
    cfg.version = WCFG_VERSION;

    set_wifi_channel(WIFI_DEFAULT_CHANNEL);
    set_sensor_type(SENSOR_TYPE_DEFAULT);
    set_LED_intensity(LED_DEFAULT_INTENSITY);
    set_config_enable(DEFAULT_IFC_ENABLE);
    set_mqtt_enable(DEFAULT_MQTT_ENABLE);

    set_display_layout(DISPLAY_LAYOUT_DEFAULT);
    set_display_parameter(0);
    set_display_timeout(T_APP_DISPLAY_TIMEOUT_S);
    set_display_contrast(DISPLAY_CONTRAST_DEFAULT);
    set_display_rotation(DISPLAY_ROTATION_DEFAULT);

    return (AppState::OK);
}

AppState SysConfig::check(void) {
    if (cfg.version != WCFG_VERSION) return (AppState::access_denied);
    if ((cfg.wifi_channel < 1) || (cfg.wifi_channel > 12)) return (AppState::invalid_arg);
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
    modified = false;
    return (check());
}

AppState SysConfig::save(void) {
    if (cfg.version != WCFG_VERSION) {
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
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "SysConfig::update()");
#endif
    AppState result = AppState::OK;
    if (modified) {
#ifdef DISPLAY_STATE
        ESP_LOGI(TAG, "SysConfig::update() -> Data modified, writing requested.");
#endif
        result = save();
        if (result == AppState::OK) {
            modified = false;
#ifdef DISPLAY_STATE
        } else {
            ESP_LOGE(TAG, "SysConfig::update() -> Write into flash has failed.");
#endif
        }
#ifdef DISPLAY_STATE
    } else {
        ESP_LOGE(TAG, "SysConfig::update() -> Flash already up-to-date.");
#endif
    }

    return (result);
}

float SysConfig::get_display_timeout(void) {
    return (cfg.display_timeout_s);
}

const char* SysConfig::get_display_timeout_str(void) {
    if (cfg.display_timeout_s <= 0.0f) {
        return ("Never");
    }
    memset(this->format, 0, sizeof (this->format));
    if (cfg.display_timeout_s >= 3600.0f) {
        snprintf(this->format, sizeof (this->format) - 1, "%.2f h", cfg.display_timeout_s / 3600.0f);
    } else if (cfg.display_timeout_s >= 60.0f) {
        snprintf(this->format, sizeof (this->format) - 1, "%.0f min", cfg.display_timeout_s / 60.0f);
    } else {
        snprintf(this->format, sizeof (this->format) - 1, "%.0f s", cfg.display_timeout_s);
    }
    return ((const char*)this->format);
}

AppState SysConfig::set_display_timeout(float _timeout_s) {
    if ((fabsf(cfg.display_timeout_s - _timeout_s) > 0.1f) && (_timeout_s >= 0.0f)) {
        cfg.display_timeout_s = _timeout_s;
        modified = true;
    }
    return (AppState::OK);
}

AppState SysConfig::set_display_timeout_str(SysConfig* _instance, const char* _str) {
    float value = 0.0f;
    if (strstr(_str, "Never") == nullptr) {
        if (strstr(_str, " min") != nullptr) {
            value = atof(_str) * 60.0f;
        } else if (strstr(_str, " h") != nullptr) {
            value = atof(_str) * 3600.0f;
        } else {
            value = atof(_str);
        }
    }
    return (_instance->set_display_timeout(value));
}

AppState SysConfig::set_wifi_channel(int _channel) {
    if ((_channel < 1) || (_channel > 12)) {
        return (AppState::invalid_arg);
    }
    if (cfg.wifi_channel != (uint8_t)_channel){
        modified = true;
    }
    cfg.wifi_channel = (uint8_t)_channel;
    return (AppState::OK);
}

AppState SysConfig::set_wifi_channel_str(SysConfig* _instance, const char* _channel) {
    int channel = (int)Tools::string2number(_channel);
    return (_instance->set_wifi_channel(channel));
}

int SysConfig::get_wifi_channel(void) {
    if (cfg.version != WCFG_VERSION) {
        return (-1);
    }
    return (cfg.wifi_channel); 
}

const char* SysConfig::get_ssid(void) { 
    if (cfg.version != WCFG_VERSION) {
        return (nullptr);
    }
    return (cfg.ssid); 
}

AppState SysConfig::set_ssid(const char* _ap_name) {
    if (_ap_name == nullptr) {
        return (AppState::invalid_arg);
    }
    if (!strncmp(cfg.ssid, _ap_name, sizeof (cfg.ssid))) {
        return (AppState::OK);
    }
    memset(cfg.ssid, 0, sizeof(cfg.ssid));
    strncpy(cfg.ssid, _ap_name, sizeof(cfg.ssid) - 1);
    modified = true;
    return (AppState::OK);
}

AppState SysConfig::set_wifi_ssid_str(SysConfig* _instance, const char* _ap_name) {
    return (_instance->set_ssid(_ap_name));
}

const char* SysConfig::get_password(void) {
    if (cfg.version != WCFG_VERSION) {
        return (nullptr);
    }
    return (cfg.password); 
}

AppState SysConfig::set_password(const char* _password) {
    if (_password == nullptr) {
        return (AppState::invalid_arg);
    }
    if (!strncmp(cfg.password, _password, sizeof (cfg.password))) {
        return (AppState::OK);
    }
    memset(cfg.password, 0, sizeof (cfg.password));
    strncpy(cfg.password, _password, sizeof (cfg.password) - 1);
    modified = true;
    return (AppState::OK);
}

AppState SysConfig::set_wifi_password_str(SysConfig* _instance, const char* _password) {
    return (_instance->set_password(_password));
}

AppState SysConfig::set_LED_intensity(float _intensity) {
    float preset = _intensity;
    if (_intensity > 1.0f) {
        preset = _intensity / 100.0f;
    }
    preset = MAX(0.0f, MIN(1.0f, preset));
    if (fabsf(cfg.led_intensity - preset) >= 0.01f) {
        modified = true;
    }
    cfg.led_intensity = preset;
    return (AppState::OK);
}

AppState SysConfig::set_LED_intensity_str(SysConfig* _instance, const char* _intensity) {
    float intensity = Tools::string2number(_intensity) + 0.001;
    return (_instance->set_LED_intensity(intensity));
}

float SysConfig::get_LED_intensity(void) {
    return (cfg.led_intensity);
}

bool SysConfig::get_config_enable(void) {
    return (cfg.f_ifc_enable == 1);
}

AppState SysConfig::set_config_enable(bool enable) {
    if (get_config_enable() != enable) {
        modified = true;
    }
    cfg.f_ifc_enable = (uint8_t)((enable) ? 1 : 0);
    return (AppState::OK);
}

bool SysConfig::get_mqtt_enable(void) {
    return (cfg.f_mqtt_enable == 1);
}

AppState SysConfig::set_mqtt_enable(bool _enable) {
    if (get_mqtt_enable() != _enable) {
        modified = true;
    }
    cfg.f_mqtt_enable = (uint8_t)((_enable) ? 1 : 0);
    return (AppState::OK);
}

AppState SysConfig::set_mqtt_enable_str(SysConfig* _instance, const char* _enable) {
    bool enable = false;
    if (strstr(_enable, "ena") != nullptr) {
        enable = true;
    } else if (strstr(_enable, "dis") != nullptr) {
        enable = false;
    } else {
        enable = (Tools::string2number(_enable) != 0.0f);
    }
    return (_instance->set_mqtt_enable(enable));
}

SensorType SysConfig::get_sensor_type(void) {
    return ((SensorType)cfg.sensor_type);
}

AppState SysConfig::set_sensor_type(SensorType _type) {
    uint8_t type = ((uint8_t)_type & 0xff);
    if (type != cfg.sensor_type) {
        cfg.sensor_type = type;
        modified = true;
        esp_event_post(APP_EVENT, (int32_t)AppEvent::driver_config, nullptr, 0, pdMS_TO_TICKS(1));
    }
    return (AppState::OK);
}

AppState SysConfig::set_sensor_type_str(SysConfig* _instance, const char* _type) {
    int type = 0;
    for (int i = 0; i < SIZEOFARRAY(SensorDriver::str_sensor_types); i++) {
        if (strcmp(_type, SensorDriver::str_sensor_types[i]) == 0) {
            type = i;
            break;
        }
    }
    return (_instance->set_sensor_type((SensorType)(type)));
}

AppState SysConfig::flip_display_rotation(void) {
    return (set_display_rotation(get_display_rotation() + 180));
}

AppState SysConfig::set_display_rotation(int _degrees) {
    uint8_t rot_step = (uint8_t)((_degrees / 90) & 0x03);
    if ((rot_step & 1) != 0) {
        return (AppState::not_implemented);
    }
    if (rot_step != cfg.f_display_rotoation) {
        cfg.f_display_rotoation = rot_step;
        modified = true;
        esp_event_post(APP_EVENT, (int32_t)AppEvent::display_config, nullptr, 0, pdMS_TO_TICKS(1));
    }
    return (AppState::OK);
}

AppState SysConfig::set_display_rotation_str(SysConfig* _instance, const char* _degrees) {
    float degrees = Tools::string2number(_degrees);
    return (_instance->set_display_rotation((int)degrees));
}

int SysConfig::get_display_rotation(void) {
    if (cfg.f_display_rotoation == 3) {
        return (270);
    } else if (cfg.f_display_rotoation == 2) {
        return (180);
    } else if (cfg.f_display_rotoation == 1) {
        return (90);
    }
    return (0);
}

AppState SysConfig::set_display_contrast(float _value) {
    float contrast = fabsf(_value);
    if (contrast > 1.0f) {
        contrast = contrast / 100.0f;
    }
    contrast = MIN(1.0f, MAX(0.0f, contrast));
    if (fabsf(cfg.display_contrast - contrast) >= 0.01f) {
        cfg.display_contrast = contrast;
        modified = true;
        esp_event_post(APP_EVENT, (int32_t)AppEvent::display_config, nullptr, 0, pdMS_TO_TICKS(1));
    }
    return (AppState::OK);
}

AppState SysConfig::set_display_contrast_str(SysConfig* _instance, const char* _value) {
    float value = Tools::string2number(_value) + 0.001f;
    return (_instance->set_display_contrast(value));
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

AppState SysConfig::set_display_layout(DisplayLayout _layout) {
    if (cfg.display_layout != (uint8_t)_layout) {
        cfg.display_layout  = (uint8_t)_layout;
        cfg.display_param   = (uint8_t)0;
        modified = true;
        esp_event_post(APP_EVENT, (int32_t)AppEvent::display_config, nullptr, 0, pdMS_TO_TICKS(1));
    }
    return (AppState::OK);
}

AppState SysConfig::set_display_layout_str(SysConfig* _instance, const char* _layout) {
    for (size_t i = 0; i < SIZEOFARRAY(SysConfig::str_display_layout); i++) {
        if (strstr(_layout, SysConfig::str_display_layout[i]) != nullptr) {
            _instance->set_display_layout((DisplayLayout)i);
            return (AppState::OK);
        }
    }
    return (AppState::invalid_arg);
}

DisplayLayout SysConfig::get_display_layout(void) {
    return ((DisplayLayout)cfg.display_layout);
}

const char* SysConfig::get_display_layout_str(void) {
    return (get_display_layout_name(get_display_layout()));
}

const char* SysConfig::get_display_layout_name(DisplayLayout __layout) {
    int index = (int)(__layout);
    if ((index >= 0) && (index < SIZEOFARRAY(SysConfig::str_display_layout))) {
        return (SysConfig::str_display_layout[index]);
    }

    return ("Invalid");
}

AppState SysConfig::set_display_parameter(int _parameter) {
    uint8_t parameter = (uint8_t)(_parameter & 0xff);
    if (cfg.display_param != parameter) {
        cfg.display_param = parameter;
        modified = true;
        esp_event_post(APP_EVENT, (int32_t)AppEvent::display_config, nullptr, 0, pdMS_TO_TICKS(1));
    }
    return (AppState::OK);
}

AppState SysConfig::set_display_parameter_str(SysConfig* _instance, const char* _parameter) {
    int parameter = (int)Tools::string2number(_parameter);
    return (_instance->set_display_parameter(parameter));
}

uint8_t SysConfig::get_display_parameter(void) {
    return (cfg.display_param);
}

AppState SysConfig::set_mqtt_broker(const char* _broker) {
    if (_broker == nullptr) {
        return (AppState::invalid_arg);
    }
    if (!strncmp(cfg.mqtt_broker, _broker, sizeof (cfg.mqtt_broker))) {
        return (AppState::OK);
    }
    memset(cfg.mqtt_broker, 0, sizeof(cfg.mqtt_broker));
    strncpy(cfg.mqtt_broker, _broker, sizeof(cfg.mqtt_broker) - 1);
    modified = true;
    return (AppState::OK);
}

AppState SysConfig::set_mqtt_broker_str(SysConfig* _instance, const char* _broker) {
    return (_instance->set_mqtt_broker(_broker));
}

const char* SysConfig::get_mqtt_broker(void) {
    return (cfg.mqtt_broker);
}

AppState SysConfig::set_mqtt_username_str(SysConfig* _instance, const char* _username) {
    return (_instance->set_mqtt_username(_username));
}

AppState SysConfig::set_mqtt_username(const char* _username) {
    if (_username == nullptr) {
        return (AppState::invalid_arg);
    }
    if (!strncmp(cfg.mqtt_username, _username, sizeof (cfg.mqtt_username))) {
        return (AppState::OK);
    }
    memset(cfg.mqtt_username, 0, sizeof (cfg.mqtt_username));
    strncpy(cfg.mqtt_username, _username, sizeof (cfg.mqtt_username) - 1);
    modified = true;
    return (AppState::OK);
}

const char* SysConfig::get_mqtt_username(void) {
    return (cfg.mqtt_username);
}

AppState SysConfig::set_mqtt_password(const char* _password) {
    if (_password == nullptr) {
        return (AppState::invalid_arg);
    }
    if (!strncmp(cfg.mqtt_password, _password, sizeof (cfg.mqtt_password))) {
        return (AppState::OK);
    }
    memset(cfg.mqtt_password, 0, sizeof (cfg.mqtt_password));
    strncpy(cfg.mqtt_password, _password, sizeof (cfg.mqtt_password) - 1);
    modified = true;
    return (AppState::OK);
}

AppState SysConfig::set_mqtt_password_str(SysConfig* _instance, const char* _password) {
    return (_instance->set_mqtt_password(_password));
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
    ESP_LOGI(TAG, "\t- Version:            0x%-8.8X ", (unsigned int)cfg.version);

    ESP_LOGI(TAG, "\t- AP SSID:            <%s> ",     get_ssid());
    ESP_LOGI(TAG, "\t- AP password:        <%s> ",     get_password());
    ESP_LOGI(TAG, "\t- AP channel:         %d ",       get_wifi_channel());

    ESP_LOGI(TAG, "\t- MQTT client:        %s ",       (get_mqtt_enable()) ? "enabled" : "disabled");
    ESP_LOGI(TAG, "\t- MQTT broker:        <%s> ",     get_mqtt_broker());
    ESP_LOGI(TAG, "\t- MQTT username:      <%s> ",     get_mqtt_username());
    ESP_LOGI(TAG, "\t- MQTT password:      <%s> ",     get_mqtt_password());

    ESP_LOGI(TAG, "\t- Display Layout:     %d ",       (int)get_display_layout());
    ESP_LOGI(TAG, "\t- Display parameter:  %d ",       (int)get_display_parameter());
    ESP_LOGI(TAG, "\t- Display rotation:   %d   ",     (int)get_display_rotation());
    ESP_LOGI(TAG, "\t- Display timeout:    %.3f s ",   get_display_timeout());
    ESP_LOGI(TAG, "\t- Display contrast:   %.1f ",     get_display_contrast());

    ESP_LOGI(TAG, "\t- Config interface:   %s ",       (get_config_enable()) ? "enabled" : "disabled");
    ESP_LOGI(TAG, "\t- Sensor type ID:     %d <%s>",   (int)get_sensor_type(), SensorDriver::get_driver_name(get_sensor_type()));

    ESP_LOGI(TAG, "\t- LED intensity:      %.2f ",     get_LED_intensity());
#endif    
}


