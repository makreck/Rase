/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
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

class ProductID : public MicroJsonObject {
    public:
        char identification[32];
        char manufacturer[32];
        char product[32];
        char device_serial_number[22];
        char firmware_version[16];
        char firmware_date[16];
        char chip_type[16];
        char wifi_station_mac[20];
        char wifi_ap_mac[20];
        char bluetooth_mac[20];
        char rssi[16];
        char tx_power[16];

        const MicroJsonStruct identificationData[12] = {
            JSON_ITEM(ProductID, identification,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, manufacturer,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, product,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, device_serial_number,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, firmware_version,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, firmware_date,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, chip_type,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, wifi_station_mac,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, wifi_ap_mac,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, bluetooth_mac,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, rssi,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(ProductID, tx_power,  MicroJsonObjectType::obj_chars),
        };
        JSON_GETTERS(ProductID, identificationData);

        bool operator==(const ProductID& _source) {
            return (strncmp(this->device_serial_number, _source.device_serial_number, sizeof (device_serial_number)) == 0);
        }

        bool is_equal_device(const ProductID* _source) {
            return (strncmp(this->device_serial_number, _source->device_serial_number, sizeof (device_serial_number)) == 0);
        }
};
