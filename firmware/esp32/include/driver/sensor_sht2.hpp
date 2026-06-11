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

#define SHT2_RESET_TIME_MS              (100)
#define SHT2_CONV_TIME_MAX_MS           (85)
#define SHT2_CONV_TIME_T_14B_MS         (85)
#define SHT2_CONV_TIME_T_13B_MS         (43)
#define SHT2_CONV_TIME_T_12B_MS         (22)
#define SHT2_CONV_TIME_T_11B_MS         (11)
#define SHT2_CONV_TIME_RH_12B_MS        (29)
#define SHT2_CONV_TIME_RH_11B_MS        (15)
#define SHT2_CONV_TIME_RH_10B_MS        (9)
#define SHT2_CONV_TIME_RH_8B_MS         (4)

#define SHT20_POLYNOMIAL                (0x131) // P(x) = x^8+x^5+x^4+1 = 100110001

#define	SHT2_RES_RH12_T14               (0x00)
#define	SHT2_RES_RH08_T12               (0x01)
#define	SHT2_RES_RH10_T13               (0x80)
#define	SHT2_RES_RH11_T11               (0x81)
#define	SHT2_RES_MASK 	                (0x81)

#define	SHT2_END_OF_BATTERY_MASK        (0x40)
#define	SHT2_HEATER_MASK                (0x04)
#define	SHT2_OTP_RELOAD_MASK            (0x02)

#define SHT2_MASK__8_BIT                (0xff00)
#define SHT2_MASK_10_BIT                (0xffC0)
#define SHT2_MASK_11_BIT                (0xffe0)
#define SHT2_MASK_12_BIT                (0xfff0)
#define SHT2_MASK_13_BIT                (0xfff8)
#define SHT2_MASK_14_BIT                (0xfffc)
#define SHT2_MASK_VALUE_BITS_MAX        (SHT2_MASK_14_BIT)
#define SHT2_MASK_STATUS                (0x0003)

enum class SHT2Command : uint8_t {

    // 8-bit standard command codes:

    SOFT_RESET                = 0xFE, // Reset command

    TRIGGER_T_NO_HOLD_MASTER  = 0xF3, // Trigger temperature measurement (Hold Master)
    TRIGGER_RH_NO_HOLD_MASTER = 0xF5, // Trigger humidity measurement (Hold Master)
    TRIGGER_T_HOLD_MASTER     = 0xE3, // Trigger temperature measurement (No Hold Master)
    TRIGGER_RH_HOLD_MASTER    = 0xE5, // Trigger humidity measurement (No Hold Master)
    
    // READ_STATUS               = 0xE0, // Read status register (probably not existing command?)
    // HEATER_ENABLE             = 0xB4, // Heater enable (probably using user-register bits to enable/disable?)
    // READ_CAL_COEFF            = 0x3A, // Read electrical calibration coefficients (probably not existing command?)

    READ_USER_REGISTER        = 0xE7, // Read User Register
    WRITE_USER_REGISTER       = 0xE6, // Write User Register

    // 16-bit special command codes:

    READ_OTP_MEMORY           = 0xFA, // Read serial number part #1 from OTP (8 data bytes as 4 pairs of (data, crc), ), 16-bit command MSB
    READ_OTP_SERIAL_START_ADR = 0x0F, // Read serial number part #1, 16-bit command LSB (start address, possibly 0x0C)
    READ_METAL_ROM            = 0xFC, // Read serial number part #2 from Metal-ROM (6 data bytes as 3 pairs of (data, crc), ), 16-bit command MSB
    READ_ROM_SERIAL_START_ADR = 0xC9, // Read serial number part #2, 16-bit command LSB (start address)

};

class SHT2_Channel {
    public:
        int      bit_resolution  = 12;
        int      conv_time_ms    = SHT2_CONV_TIME_MAX_MS;
        uint16_t value_mask      = SHT2_MASK_VALUE_BITS_MAX;
        uint16_t raw_data        = 0;

        void setup(int _bit_resolution, int _conv_time_ms, uint16_t _value_mask = 0xfffc) {
            bit_resolution  = _bit_resolution;
            conv_time_ms    = _conv_time_ms;
            value_mask      = _value_mask;
            raw_data        = 0;
        }

        uint16_t get_raw_value(void) {
            return (raw_data & value_mask);
        }

        uint16_t get_status(void) {
            return (raw_data & SHT2_MASK_STATUS);
        }

        float get_value(void) {
            return ((float)get_raw_value() / 65536.0f);
        }
};

class SensorSHT2 : public SensorDriver {
    private:
        static const SensorProperty sht2_properties[];

        const char* headName   = SHT2_DEVICE_NAME;
        char device_serial_number[16]{ 0 };


        float temperature_C     = 0.0f;
        float pressure_hPa      = 0.0f;
        float humidity_RH       = 0.0f;
        float dewpoint_C        = 0.0f;
        float humidity_AH       = 0.0f;

        uint8_t user_register   = 0x00;
        bool disable_OTP_reload = false;
        bool enable_heater      = false;
        bool end_of_battery     = false;
        
        struct {
            SHT2_Channel RH;
            SHT2_Channel T;
        } channel;

        esp_err_t init(void);
        esp_err_t cleanup(void);
        
        esp_err_t enable(void) override;
        esp_err_t disable(void) override;
        esp_err_t fetch(void) override;
        esp_err_t update(void) override;

        esp_err_t write_user_register(uint8_t data);
        esp_err_t read_user_register(uint8_t* data = nullptr);

        esp_err_t soft_reset(void);
        esp_err_t read_measurement(uint8_t cmd, uint32_t wait_ms, uint16_t& raw_value);

    public:
        SensorSHT2() {
            init();
        }

        ~SensorSHT2() override {
            cleanup();
        }

        static bool is_connected(SwI2CBus& bus);
        static bool check_crc(uint8_t* data, uint8_t length, uint8_t checksum);
        static esp_err_t read_serial_number_code(SwI2CBus& bus, uint8_t address, uint32_t& serialNumberCode, char* serialNumberString = nullptr, size_t snLength = 0);

        const SensorProperty* get_properties(void) override;
        size_t get_property_count(void) override;
        esp_err_t set_heating(bool enable) override;
        const char* get_head(void) override;
        SensorType get_sensor_type(void) override;
        uint8_t get_sub_type(void) override;
};    
