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

#define INVALID_DEVICE_ADDRESS          (0xff)

#define MULTI_ADDR_HDC1080_HTU21_SHT2X  (0x40)
#define MULTI_ADDR_BMP280_BME280        (0x76)
#define MULTI_ADDR_AHT10_AHT21          (0x38)

#define AHT10_DEVICE_ADDRESS            (0x38)
#define AHT15_DEVICE_ADDRESS            (0x38)
#define AHT21_DEVICE_ADDRESS            (0x38)
#define HTU21_DEVICE_ADDRESS            (0x40)
#define HDC1080_DEVICE_ADDRESS          (0x40)
#define SHT2_DEVICE_ADDRESS             (0x40)
#define SI7021_DEVICE_ADDRESS           (0x40)
#define SHT3_DEVICE_ADDRESS             (0x44)
#define SHT8_DEVICE_ADDRESS             (0x44)
#define BMP280_DEVICE_ADDRESS           (0x76)
#define BME280_DEVICE_ADDRESS           (0x76)

// #define I2C_BUS_BROADCAST_ADDRESS   (0x77)

#define SHT2_DEVICE_NAME                "SHT2x"
#define SHT3_DEVICE_NAME                "SHT3x"
#define HTU21_DEVICE_NAME               "HTU21d"
#define AHT_DEVICE_NAME                 "AHTxx"
#define AHT10_DEVICE_NAME               "AHT10"
#define AHT21_DEVICE_NAME               "AHT21"
#define HDC1080_DEVICE_NAME             "HDC10x"
#define BMX280_DEVICE_NAME              "BMx280"
#define BMP280_DEVICE_NAME              "BMP280"
#define BME280_DEVICE_NAME              "BME280"

#define ADR_38_DEV_NAMES                "AHTxx"
#define ADR_40_DEV_NAMES                "SHT2/HTU/HDC"
#define ADR_44_DEV_NAMES                "SHT3x"
#define ADR_76_DEV_NAMES                "BMP/BME"

enum class SensorType : uint8_t {
    autoscan = 0x00,
    Null     = 0x01,
    SHT2x    = 0x02,
    SHT3x    = 0x03,
    HTU21d   = 0x04,
    ATHxx    = 0x05,
    HDC1080  = 0x06,
    BMx280   = 0x07,
    invalid  = 0xff,
};
