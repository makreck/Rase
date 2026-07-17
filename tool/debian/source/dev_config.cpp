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

#include "includes.h"

bool App::read_config(void) {
    char* config_json = transact_command("/config");
    if (config_json != nullptr) {
        memset(&m.device.cfg, 0, sizeof (m.device.cfg));

        KeyList key_list[] = {
            { "version",              m.device.cfg.version,             sizeof (m.device.cfg.version)           },
            { "ssid",                 m.device.cfg.wifi_ssid,           sizeof (m.device.cfg.wifi_ssid)         },
            { "password",             m.device.cfg.wifi_password,       sizeof (m.device.cfg.wifi_password)     },
            { "wifi_channel",         m.device.cfg.wifi_channel,        sizeof (m.device.cfg.wifi_channel)      },
            { "mqtt_broker",          m.device.cfg.mqtt_broker,         sizeof (m.device.cfg.mqtt_broker)       },
            { "mqtt_username",        m.device.cfg.mqtt_username,       sizeof (m.device.cfg.mqtt_username)     },
            { "mqtt_password",        m.device.cfg.mqtt_password,       sizeof (m.device.cfg.mqtt_password)     },
            { "mqtt_enable",          m.device.cfg.mqtt_enable,         sizeof (m.device.cfg.mqtt_enable)       },
            { "display_layout",       m.device.cfg.display_layout,      sizeof (m.device.cfg.display_layout)    },
            { "display_param",        m.device.cfg.display_param,       sizeof (m.device.cfg.display_param)     },
            { "display_rotation",     m.device.cfg.display_rotoation,   sizeof (m.device.cfg.display_rotoation) },
            { "display_timeout",      m.device.cfg.display_timeout_s,   sizeof (m.device.cfg.display_timeout_s) },
            { "display_contrast",     m.device.cfg.display_contrast,    sizeof (m.device.cfg.display_contrast)  },
            { "sensor_type",          m.device.cfg.sensor_type,         sizeof (m.device.cfg.sensor_type)       },
            { "led_intensity",        m.device.cfg.led_intensity,       sizeof (m.device.cfg.led_intensity)     },
        };
        import_data(config_json, key_list, SIZEOFARRAY(key_list));

        free(config_json);
        return (true);
    }
    return (false);
}

bool App::read_id(void) {
    char* id_json = transact_command("/api/id");
    if (id_json != nullptr) {
        memset(&m.device.id, 0, sizeof (m.device.id));

        KeyList key_list[] = {
            { "identification",       m.device.id.identification,       sizeof (m.device.id.identification)       },
            { "manufacturer",         m.device.id.manufacturer,         sizeof (m.device.id.manufacturer)         },
            { "product",              m.device.id.product,              sizeof (m.device.id.product)              },
            { "device_serial_number", m.device.id.device_serial_number, sizeof (m.device.id.device_serial_number) },
            { "firmware_version",     m.device.id.firmware_version,     sizeof (m.device.id.firmware_version)     },
            { "firmware_date",        m.device.id.firmware_date,        sizeof (m.device.id.firmware_date)        },
            { "sensor_head",          m.device.id.head,                 sizeof (m.device.id.head)                 },
            { "head_serial_number",   m.device.id.head_serial,          sizeof (m.device.id.head_serial)          },
            { "chip_type",            m.device.id.chip_type,            sizeof (m.device.id.chip_type)            },
            { "wifi_station_mac",     m.device.id.wifi_station_mac,     sizeof (m.device.id.wifi_station_mac)     },
            { "wifi_ap_mac",          m.device.id.wifi_ap_mac,          sizeof (m.device.id.wifi_ap_mac)          },
            { "bluetooth_mac",        m.device.id.bluetooth_mac,        sizeof (m.device.id.bluetooth_mac)        },
            { "ip_addr",              m.device.id.ip_addr,              sizeof (m.device.id.ip_addr)              },
            { "rssi",                 m.device.id.rssi,                 sizeof (m.device.id.rssi)                 },
            { "tx_power",             m.device.id.tx_power,             sizeof (m.device.id.tx_power)             },
        };
        import_data(id_json, key_list, SIZEOFARRAY(key_list));

        free(id_json);
        return (true);
    }
    return (false);
}
