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

const char* intrinsic_date_month_names[] = { "not-a-month", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", };

const char* device_id_json =
    "{\n"
    "\t\"identification\": \"" SENSOR_ID "\",\n"
    "\t\"manufacturer\": \"" MANUFACTURER_ID "\",\n"
    "\t\"product\": \"" PRODUCT_ID "\",\n"
    "\t\"device_serial_number\": \"%s\",\n"
    "\t\"firmware_version\": \"%s\",\n"
    "\t\"firmware_date\": \"%s\",\n"
    "\t\"chip_type\": \"" CHIP_TYPE "\",\n"
    "\t\"wifi_station_mac\": \"%s\",\n"
    "\t\"wifi_ap_mac\": \"%s\",\n"
    "\t\"bluetooth_mac\": \"%s\",\n"
    "\t\"rssi\": \"%s\",\n"
    "\t\"tx_power\": \"%s\",\n"
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

char* Tools::get_device_id_json(void) {
    char device_serial_number[22]{0};
    char firmware_version[16]{0};
    char iso_firmware_date[16]{0};
    char wifi_ap_mac[20]{0};
    char wifi_sta_mac[20]{0};
    char bt_mac[20]{0};
    char rssi_string[16]{ 0 };
    char tx_power_string[16]{ 0 };

    Tools::get_device_serial_number(device_serial_number, sizeof (device_serial_number));

    snprintf(firmware_version, sizeof (firmware_version), "%d.%d.%d.%d", VS_HIGH, VS_LOW, VS_REV, VS_BUILD);

    get_iso_build_date(iso_firmware_date, sizeof (iso_firmware_date));

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(wifi_sta_mac, sizeof (wifi_sta_mac), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    snprintf(wifi_ap_mac, sizeof (wifi_ap_mac), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    esp_read_mac(mac, ESP_MAC_BT);
    snprintf(bt_mac, sizeof (bt_mac), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    Wifi_Station::get_rssi_dbm(rssi_string, sizeof (rssi_string));
    Wifi_Station::get_tx_power_dbm(tx_power_string, sizeof (tx_power_string));

    size_t length = snprintf(nullptr, 0, device_id_json, device_serial_number,
        firmware_version, iso_firmware_date, wifi_sta_mac, wifi_ap_mac, bt_mac, rssi_string, tx_power_string);

    char* json_string = (char*)malloc(length + 1);

    snprintf(json_string, length + 1, device_id_json, device_serial_number,
        firmware_version, iso_firmware_date, wifi_sta_mac, wifi_ap_mac, bt_mac, rssi_string, tx_power_string);

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
        "%04d-%02d-%02d%c%02d:%02d:%02d", // .%03d",
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        div_char,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);
        // (int)(tv.tv_usec / 1000));

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
