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

#define HDC1080_REG_TEMPERATURE     (0x00) // read temperature output, 2 bytes, MSB -> LSB
#define HDC1080_REG_HUMIDITY        (0x01) // read humidity output, 2 bytes, MSB -> LSB
#define HDC1080_REG_CONFIG_STATUS   (0x02) // read/write config and status register, 2 bytes data
#define HDC1080_REG_SERIAL_ID_A     (0xFB) // serial number output part 1, read only, bytes 5 and 4
#define HDC1080_REG_SERIAL_ID_B     (0xFC) // serial number output part 2, read only, bytes 3 and 2
#define HDC1080_REG_SERIAL_ID_C     (0xFD) // serial number output part 3, read only, bytes 1 and 0
#define HDC1080_REG_MANUFACTURER_ID (0xFE) // Texas Instruments = 0x5449
#define HDC1080_REG_DEVICE_ID       (0xFF) // Device ID, read only, HDC1080 = 0x1050

#define HDC1080_CFR_RST             (0x8000) // write: reset device, read: 0 = normal operation
#define HDC1080_CFR_RES14           (0x4000) // reserved bit
#define HDC1080_CFR_HEAT_ON         (0x2000) // heating enable (1) or disable (0)
#define HDC1080_CFR_HEAT_OFF        (0x0000) // heating enable (1) or disable (0)
#define HDC1080_CFR_MODE_AQ         (0x0000) // T/RH aquiered (0), T/RH aquired in sequence (1)
#define HDC1080_CFR_MODE_SEQ        (0x1000) // T/RH aquiered (0), T/RH aquired in sequence (1)
#define HDC1080_CFR_BTST            (0x0800) // Battery state (read only), 0 = V > 2.8V, 1 = V < 2.8V
#define HDC1080_CFR_TRES_14BIT      (0x0000) // Temperature resolution, 0 = 14-bit, 1 = 11-bit
#define HDC1080_CFR_TRES_11BIT      (0x0400) // Temperature resolution, 0 = 14-bit, 1 = 11-bit
#define HDC1080_CFR_HRES_14BIT      (0x0000) // Humidity resolution (config bits 9:8), 00 = 14 bits
#define HDC1080_CFR_HRES_11BIT      (0x0100) // Humidity resolution (config bits 9:8), 01 = 11 bits
#define HDC1080_CFR_HRES_8BIT       (0x0200) // Humidity resolution (config bits 9:8), 10 = 8 bit
#define HDC1080_CFR_RES_0_7         (0x0000) // Bits 0 to 7 are reserved and must be 0

#define HDC1080_CFR_DEFAULT         (HDC1080_CFR_HEAT_OFF | HDC1080_CFR_MODE_AQ | HDC1080_CFR_TRES_14BIT | HDC1080_CFR_HRES_14BIT)
#define HDC1080_CFR_DEFAULT_MSB     ((HDC1080_CFR_DEFAULT >> 8) & 0xff)
#define HDC1080_CFR_DEFAULT_LSB     ((HDC1080_CFR_DEFAULT >> 0) & 0xff)

#define HDC1080_MANUF_ID_TI         (0x5449) // Texas Instruments manufacturer ID code
#define HDC1080_DEVICE_ID           (0x1050) // Ti HDC1080 device ID code

#define HDC1080_CONVERSION_TIME_MS  (10)     // conversion time for humidity max = 6.5 ms, for temperature max = 6.35 ms
#define HDC1080_RESET_TIME_MS       (1000)   // estimated, no info

class SensorHDC1080 : public SensorDriver {
    private:
        static const SensorProperty hdc1080_properties[];
    
        const char* headName    = HDC1080_DEVICE_NAME;
        uint16_t deviceID       = 0;
        uint16_t manufacturerID = 0;
        uint16_t dataWord       = 0;
        uint16_t rawRH          = 0;
        uint16_t rawTemp        = 0;
        float temperature_C     = 0.0f;
        float humidity_RH       = 0.0f;
        float dewpoint_C        = 0.0f;
        float humidity_AH       = 0.0f;

        esp_err_t init(void);
        esp_err_t cleanup(void);

        esp_err_t reset_device(void);
        esp_err_t read_config_status(uint16_t& dataWord);
        esp_err_t write_config_status(const uint16_t dataWord);
        esp_err_t read_temperature(uint16_t& rawTemp);
        esp_err_t read_humidity(uint16_t& rawRH);
        esp_err_t read_serial_number48(uint64_t& serialNumberCode);
        esp_err_t read_serial_number(uint32_t& serialNumberCode);
        esp_err_t read_device_id(uint16_t& deviceID);
        esp_err_t read_manufacturer_id(uint16_t& manufacturerID);
        esp_err_t read_serial_number_code(uint32_t& serialNoCode);

        esp_err_t enable(void) override;
        esp_err_t disable(void) override;
        esp_err_t fetch(void) override;
        esp_err_t update(void) override;
        
    public:

        SensorHDC1080() {
            init();
        }

        ~SensorHDC1080() override {
            cleanup();
        }

        static bool is_connected(SwI2CBus& bus);

        const SensorProperty* get_properties(void) override;
        size_t get_property_count(void) override;
        esp_err_t set_heating(bool enable);
        const char* get_head(void) override;
        SensorType get_sensor_type(void) override;
        uint8_t get_sub_type(void) override;
};    
