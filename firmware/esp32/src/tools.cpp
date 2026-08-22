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

#include "includes.hpp"
#include "app.hpp"

// #define DISPLAY_STATE

const char* Tools::intrinsic_date_month_names[] = { "not-a-month", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", };

const char* Tools::device_id_json =
    "{\n"
    "\t\"identification\": \"" SENSOR_ID "\",\n"
    "\t\"manufacturer\": \"" MANUFACTURER_ID "\",\n"
    "\t\"product\": \"" PRODUCT_ID "\",\n"
    "\t\"device_serial_number\": \"%s\",\n"
    "\t\"firmware_version\": \"%s\",\n"
    "\t\"firmware_date\": \"%s\",\n"
    "\t\"chip_type\": \"" CHIP_TYPE "\",\n"
    "\t\"sensor_head\": \"%s\",\n"
    "\t\"head_serial_number\": \"%s\",\n"
    "\t\"wifi_station_mac\": \"%s\",\n"
    "\t\"wifi_ap_mac\": \"%s\",\n"
    "\t\"bluetooth_mac\": \"%s\",\n"
    "\t\"ip_addr\": \"%s\",\n"

    "\t\"partition-label\": \"%s\",\n"
    "\t\"partition-size\": \"%.1f MB\",\n"
    "\t\"flash-chip-id\": \"0x%-8.8X\",\n"
    "\t\"flash-chip-size\": \"%.1f MB\",\n"
    "\t\"spi-ram\": \"%s\",\n"
    "\t\"heap-size-kb\": \"%s\",\n"
    
    "\t\"display\": \"%s\",\n"

    "\t\"rssi\": \"%s\",\n"
    "\t\"tx_power\": \"%s\",\n"
    "\t\"system_time\": \"%s\"\n"
    "}\n";

const char* Tools::reboot_json =
    "{\n"
    "\t\"device_serial_number\": \"%s\",\n"
    "\t\"reboot_time_stamp\": \"%s\"\n"
    "}\n";


size_t Tools::get_device_serial_number(char* buffer, size_t size) {
    uint8_t mac[6];
    if (esp_efuse_mac_get_default(mac) == ESP_OK) {
        int check = 0;
        for (int i = 0; i < 6; i++) {
            check = (check ^ (int)mac[i]) + 1;
        }
        size_t length = (size_t)snprintf(buffer, size,
            "%-3.3d%-3.3d%-3.3d%-3.3d%-3.3d%-3.3d%2.2d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], check % 100);
        return (length);
    }
    return (0);
}

char* Tools::get_reboot_json(void) {
    char device_serial_number[22]{0};
    char system_time_string[32]{ 0 };

    Tools::get_device_serial_number(device_serial_number, sizeof (device_serial_number));
    Tools::get_timestamp(system_time_string, sizeof (system_time_string));

    size_t length = snprintf(nullptr, 0, reboot_json, device_serial_number, system_time_string);

    char* json_string = (char*)malloc(length + 1);

    snprintf(json_string, length + 1, reboot_json, device_serial_number, system_time_string);

    return (json_string);
}

char* Tools::get_device_id_json(const char* _ip_addr, SensorDriver* _driver, DisplayI2C* _display) {
    char device_serial_number[22]{0};
    char firmware_version[16]{0};
    char iso_firmware_date[16]{0};
    char wifi_ap_mac[20]{0};
    char wifi_sta_mac[20]{0};
    char bt_mac[20]{0};
    char rssi_string[16]{ 0 };
    char tx_power_string[16]{ 0 };
    char ip_addr[32]{ 0 };
    char spi_ram[32]{ 0 };
    char heap_size[32]{ 0 };
    char head[16]{ 0 };
    char head_serial[16]{ 0 };
    char system_time_string[32]{ 0 };

    if (_ip_addr != nullptr) {
        strncpy(ip_addr, _ip_addr, sizeof (ip_addr) - 1);
    } else {
        strncpy(ip_addr, "0.0.0.0", sizeof (ip_addr));
    }

    if (_driver != nullptr) {
        strncpy(head, _driver->get_head(), sizeof (head) - 1);
        snprintf(head_serial, sizeof (head_serial), "0x%-8.8X", (unsigned int)_driver->get_head_serial_number());
    } else {
        strncpy(head, "autoscan", sizeof (head));
        strncpy(head_serial, "0x00000000", sizeof (head_serial));
    }

    const char* display_type;
    if (_display != nullptr) {
        display_type = _display->get_type_info();
        if (display_type == nullptr) {
            display_type = "No display";
        }
    } else {
        display_type = "unknown";
    }

    Tools::get_device_serial_number(device_serial_number, sizeof (device_serial_number));
    Tools::get_timestamp(system_time_string, sizeof (system_time_string));
    Tools::get_iso_build_date(iso_firmware_date, sizeof (iso_firmware_date));

    snprintf(firmware_version, sizeof (firmware_version), "%d.%d.%d.%d", VS_HIGH, VS_LOW, VS_REV, VS_BUILD);

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(wifi_sta_mac, sizeof (wifi_sta_mac), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    snprintf(wifi_ap_mac, sizeof (wifi_ap_mac), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    esp_read_mac(mac, ESP_MAC_BT);
    snprintf(bt_mac, sizeof (bt_mac), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    const esp_partition_t* partition = esp_ota_get_running_partition();
    if (partition == nullptr) { return (nullptr); }

    float part_size_mb = (float)partition->size / 1024.0f / 1024.0f;
    float chip_size_mb = (float)partition->flash_chip->size / 1024.0f / 1024.0f;
    
#if CONFIG_SPIRAM
    size_t spiram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    snprintf(spi_ram, sizeof (spi_ram), "%u KB", (unsigned int)(spiram_size / 1024));
#else
    strncpy(spi_ram, "disabled", sizeof (spi_ram));
#endif

    size_t total_heap = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    snprintf(heap_size, sizeof (heap_size), "%u KB", (unsigned int)(total_heap / 1024));

    Wifi_Station::get_rssi_dbm(rssi_string, sizeof (rssi_string));
    Wifi_Station::get_tx_power_dbm(tx_power_string, sizeof (tx_power_string));

    size_t length = snprintf(nullptr, 0, device_id_json, device_serial_number,
        firmware_version, iso_firmware_date, head, head_serial, wifi_sta_mac, wifi_ap_mac, bt_mac, ip_addr, 
        partition->label, part_size_mb, (unsigned int)partition->flash_chip->chip_id, chip_size_mb, spi_ram, heap_size,
        display_type, rssi_string, tx_power_string, system_time_string);

    char* json_string = (char*)malloc(length + 1);

    snprintf(json_string, length + 1, device_id_json, device_serial_number,
        firmware_version, iso_firmware_date, head, head_serial, wifi_sta_mac, wifi_ap_mac, bt_mac, ip_addr,
        partition->label, part_size_mb, (unsigned int)partition->flash_chip->chip_id, chip_size_mb, spi_ram, heap_size,
        display_type, rssi_string, tx_power_string, system_time_string);

    return (json_string);
}

void Tools::timedate(tm& timeinfo) {
    timeval tv;
    gettimeofday(&tv, nullptr);
    time_t now = tv.tv_sec;
    localtime_r(&now, &timeinfo);
}

AppState Tools::get_timestamp(char* buffer, size_t size, char div_char) {
    tm timeinfo;
    Tools::timedate(timeinfo);

    snprintf(buffer, size,
        "%04d-%02d-%02d%c%02d:%02d:%02d",
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        div_char,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);

    return (AppState::OK);
}

AppState Tools::get_time(char* buffer, size_t size) {
    tm timeinfo;
    Tools::timedate(timeinfo);

    snprintf(buffer, size,
        "%02d:%02d:%02d",
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);

    return (AppState::OK);
}

AppState Tools::get_date(char* buffer, size_t size) {
    tm timeinfo;
    Tools::timedate(timeinfo);

    snprintf(buffer, size,
        "%04d-%02d-%02d",
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday);

    return (AppState::OK);
}

uint64_t Tools::get_tickcount64(void) {
    timeval tv;
    gettimeofday(&tv, nullptr);
    return (uint64_t)(((double)tv.tv_sec * 1000.0) + ((double)tv.tv_usec / 1000.0));
}

uint32_t Tools::get_tick_seconds(void) {
    timeval tv;
    gettimeofday(&tv, nullptr);
    return ((uint32_t)tv.tv_sec);
}

float Tools::get_random(void) {
    return ((float)esp_random() / (float)UINT32_MAX);
}

const char* Tools::get_build_date(void) {
    return (__DATE__);
}

void Tools::get_iso_build_date(char* buffer, size_t size) {
    const char* p = __DATE__;
    int month = 0;
    for (month = 0; month < 13; month++) {
        if (strstr(p, intrinsic_date_month_names[month]) != nullptr) {
            break;
        }
    }
    if ((month >= 1) && (month <= 12)) {
        int day = atoi(&p[4]);
        int year = atoi(&p[(day < 10) ? 6 : 7]);
        snprintf(buffer, size - 1, "%4.4d-%2.2d-%2.2d", year, month, day);
    }
}

size_t Tools::json_get(const char* json_data, const char* _key, char* _buffer, size_t _length) {
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

float Tools::string2number(const char* _string) {
    if (_string == nullptr) {
        return (0.0f);
    }

    char format[32]{ 0 };
    strncpy(format, _string, sizeof (format) - 1);
    int len = (int)strlen(format);

    char decimal = '\0';
    for (int i = (len - 1); (i >= 0) && (decimal == '\0'); i--) {
        if ((format[i] == '.') && (decimal == '\0')) { decimal = format[i]; }
        if ((format[i] == ',') && (decimal == '\0')) { decimal = format[i]; }
    }

    const char* valid_num = "0123456789ABCDEFabcdefx+-";
    int k = 0;
    char c[2]{ 0 };
    for (int i = 0; i < len; i++) {
        c[0] = format[i];
        if ((strstr(valid_num, c) == nullptr) && (c[0] != decimal)) {
            continue;
        }
        if (i != k) {
            if (c[0] == decimal) {
                format[k] = '.';
            } else {
                format[k] = format[i];
            }
        }
        k++;
    }
    format[k] = '\0';

    float value = 0.0f;
    if (decimal != '\0') {
        value = atof(format);
    } else {
        if (strstr(format, "0x")) {
            value = (float)strtoul(format, nullptr, 0);
        } else {
            value = (float)atoi(format);
        }
    }

#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "Tools::string2number(\"%s\" -> \"%s\" = %g ", _string, format, value);
#endif

    return (value);
}

const esp_partition_t* Tools::get_next_ota_partition(void) {
    const esp_partition_t* running_partition = esp_ota_get_running_partition();
    if (running_partition == nullptr) {
        return (nullptr);
    }
    
    const esp_partition_t* ota_data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, nullptr);
    if (ota_data_partition == nullptr) {
        return (nullptr);
    }
    
    const esp_partition_t* target_partition = nullptr;
    if (running_partition->subtype == ESP_PARTITION_SUBTYPE_APP_OTA_0) {
        target_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, nullptr);
    } else if (running_partition->subtype == ESP_PARTITION_SUBTYPE_APP_OTA_1) {
        target_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, nullptr);
    }
    
    if (target_partition == nullptr) {
        return (nullptr);
    }

    return (target_partition);
}
