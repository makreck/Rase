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

// #define DISABLE_FILTER

#include "includes.h"

const char* DevConfig::config_json_cmd_format =
    "%s{\n"
    "\t\"" JSON_KEY_VERSION "\": \"%s\",\n"
    "\t\"" JSON_KEY_WIFI_SSID "\": \"%s\",\n"
    "\t\"" JSON_KEY_WIFI_PASSWORD "\": \"%s\",\n"
    "\t\"" JSON_KEY_WIFI_CHANNEL "\": \"%s\",\n"
    "\t\"" JSON_KEY_MQTT_BROKER "\": \"%s\",\n"
    "\t\"" JSON_KEY_MQTT_USERNAME "\": \"%s\",\n"
    "\t\"" JSON_KEY_MQTT_PASSWORD "\": \"%s\",\n"
    "\t\"" JSON_KEY_MQTT_ENABLE "\": \"%s\",\n"
    "\t\"" JSON_KEY_DISPLAY_LAYOUT "\": \"%s\",\n"
    "\t\"" JSON_KEY_DISPLAY_PARAM "\": \"%s\",\n"
    "\t\"" JSON_KEY_DISPLAY_ROTATION "\": \"%s\",\n"
    "\t\"" JSON_KEY_DISPLAY_TIMEOUT "\": \"%s\",\n"
    "\t\"" JSON_KEY_DISPLAY_CONTRAST "\": \"%s\",\n"
    "\t\"" JSON_KEY_SENSOR_TYPE "\": \"%s\",\n"
    "\t\"" JSON_KEY_LED_INTENSITY "\": \"%s\"\n"
    "}\n";

void DevConfig::clear(void) {
    memset(&cfg, 0, sizeof (cfg));
    memset(&id,  0, sizeof (id));
}

size_t DevConfig::json_get(char* json_data, const char* _key, char* _buffer, size_t _length) {
    if (_key == nullptr) {
        return (0);
    }

    char key[64]{ 0 };
    snprintf(key, sizeof (key), "\"%s\"", _key);

    char* p = strstr(json_data, key);
    if (p == nullptr) { return (0); }
    p += strlen(key);

    p = strstr(p, ":");
    if (p == nullptr) { return (0); }
    p += 1;
    
    p = strstr(p, "\"");
    if (p == nullptr) { return (0); }
    p += 1;
    
    size_t i;
    for (i = 0; (i < (_length - 1)) && (*p != '\"'); i++) {
        _buffer[i] = *p++;
    }
    _buffer[i] = '\0';

    return (i);
}

void DevConfig::import_data(char* _json_string, KeyList* _key_list, size_t _size) {
    char user_par[64]{ 0 };
    size_t len;
    for (int i = 0; i < _size; i++) {
        len = DevConfig::json_get(_json_string, _key_list[i].key, user_par, sizeof (user_par));
        if (len > 0) {
            strncpy(_key_list[i].field, user_par, _key_list[i].length - 1);
        }
    }
}

bool DevConfig::parse_config_json(char* _config_json) {
    memset(&cfg, 0, sizeof (cfg));

    KeyList key_list[] = {
        { JSON_KEY_VERSION,          cfg.version,             sizeof (cfg.version)           },
        { JSON_KEY_WIFI_SSID,        cfg.wifi_ssid,           sizeof (cfg.wifi_ssid)         },
        { JSON_KEY_WIFI_PASSWORD,    cfg.wifi_password,       sizeof (cfg.wifi_password)     },
        { JSON_KEY_WIFI_CHANNEL,     cfg.wifi_channel,        sizeof (cfg.wifi_channel)      },
        { JSON_KEY_MQTT_BROKER,      cfg.mqtt_broker,         sizeof (cfg.mqtt_broker)       },
        { JSON_KEY_MQTT_USERNAME,    cfg.mqtt_username,       sizeof (cfg.mqtt_username)     },
        { JSON_KEY_MQTT_PASSWORD,    cfg.mqtt_password,       sizeof (cfg.mqtt_password)     },
        { JSON_KEY_MQTT_ENABLE,      cfg.mqtt_enable,         sizeof (cfg.mqtt_enable)       },
        { JSON_KEY_DISPLAY_LAYOUT,   cfg.display_layout,      sizeof (cfg.display_layout)    },
        { JSON_KEY_DISPLAY_PARAM,    cfg.display_param,       sizeof (cfg.display_param)     },
        { JSON_KEY_DISPLAY_ROTATION, cfg.display_rotoation,   sizeof (cfg.display_rotoation) },
        { JSON_KEY_DISPLAY_TIMEOUT,  cfg.display_timeout_s,   sizeof (cfg.display_timeout_s) },
        { JSON_KEY_DISPLAY_CONTRAST, cfg.display_contrast,    sizeof (cfg.display_contrast)  },
        { JSON_KEY_SENSOR_TYPE,      cfg._sensor_type_list,   sizeof (cfg._sensor_type_list) },
        { JSON_KEY_LED_INTENSITY,    cfg.led_intensity,       sizeof (cfg.led_intensity)     },
    };
    DevConfig::import_data(_config_json, key_list, SIZEOFARRAY(key_list));

    char* p = strstr(cfg._sensor_type_list, ",");
    if (p != nullptr) {
        *p++ = '\0';
        strncpy(cfg.sensor_type, cfg._sensor_type_list, sizeof (cfg.sensor_type));
        for (int i = 0; i < sizeof (cfg._sensor_type_list); i++) {
            cfg._sensor_type_list[i] = p[i];
            if (p[i] == '\0') break;
            if (p[i] == ',') cfg._sensor_type_list[i] = '\n';
        }
    } else {
        strncpy(cfg.sensor_type, cfg._sensor_type_list, sizeof (cfg.sensor_type));
        strncpy(cfg._sensor_type_list, APPSTRING(IDS_LIST_SENSOR_TYPES), sizeof (cfg._sensor_type_list));
    }

    return (true);
}

bool DevConfig::parse_id_json(char* _id_json) {
    if (_id_json == nullptr) {
        return (false);
    }

    memset(&id, 0, sizeof (id));

    KeyList key_list[] = {
        { "identification",       id.identification,       sizeof (id.identification)       },
        { "manufacturer",         id.manufacturer,         sizeof (id.manufacturer)         },
        { "product",              id.product,              sizeof (id.product)              },
        { "device_serial_number", id.device_serial_number, sizeof (id.device_serial_number) },
        { "firmware_version",     id.firmware_version,     sizeof (id.firmware_version)     },
        { "firmware_date",        id.firmware_date,        sizeof (id.firmware_date)        },
        { "sensor_head",          id.head,                 sizeof (id.head)                 },
        { "head_serial_number",   id.head_serial,          sizeof (id.head_serial)          },
        { "chip_type",            id.chip_type,            sizeof (id.chip_type)            },
        { "wifi_station_mac",     id.wifi_station_mac,     sizeof (id.wifi_station_mac)     },
        { "wifi_ap_mac",          id.wifi_ap_mac,          sizeof (id.wifi_ap_mac)          },
        { "bluetooth_mac",        id.bluetooth_mac,        sizeof (id.bluetooth_mac)        },
        { "ip_addr",              id.ip_addr,              sizeof (id.ip_addr)              },
        { "rssi",                 id.rssi,                 sizeof (id.rssi)                 },
        { "tx_power",             id.tx_power,             sizeof (id.tx_power)             },
        { "system_time",          id.system_time,          sizeof (id.system_time)          },
    };
    DevConfig::import_data(_id_json, key_list, SIZEOFARRAY(key_list));

    return (true);
}

char* DevConfig::get_config_json(const char* _command, size_t* _length) {
    if (_command == nullptr) {
        _command = "\0";
    }

    size_t length = snprintf(nullptr, 0, config_json_cmd_format,
        _command,
        cfg.version, cfg.wifi_ssid, cfg.wifi_password, cfg.wifi_channel,
        cfg.mqtt_broker, cfg.mqtt_username, cfg.mqtt_password, cfg.mqtt_enable,
        cfg.display_layout, cfg.display_param, cfg.display_rotoation, cfg.display_timeout_s, cfg.display_contrast,
        cfg.sensor_type, cfg.led_intensity);

    size_t size = length + 8;
    char* json_string = (char*)malloc(size);
    if (json_string == nullptr) {
        return (nullptr);
    }
    memset(json_string, 0, size);

    snprintf(json_string, length + 1, config_json_cmd_format,
        _command,
        cfg.version, cfg.wifi_ssid, cfg.wifi_password, cfg.wifi_channel,
        cfg.mqtt_broker, cfg.mqtt_username, cfg.mqtt_password, cfg.mqtt_enable,
        cfg.display_layout, cfg.display_param, cfg.display_rotoation, cfg.display_timeout_s, cfg.display_contrast,
        cfg.sensor_type, cfg.led_intensity);

    if (_length != nullptr) {
        *_length = length;
    }

    return (json_string);
}
