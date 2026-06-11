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

#define BM2_MSBLSB_TO_U16(msb, lsb) (((uint16_t)msb << 8) | ((uint16_t)lsb))

#define BM2_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH  (26)
#define BM2_HUMIDITY_CALIB_DATA_LENGTH              (7)
#define BM2_GEN_READ_WRITE_DATA_LENGTH              (1)
#define BM2_HUMIDITY_DATA_LENGTH                    (2)
#define BM2_TEMPERATURE_DATA_LENGTH                 (3)
#define BM2_PRESSURE_DATA_LENGTH                    (3)
#define BM2_ALL_DATA_FRAME_LENGTH                   (8)

#define	BM2_DATA_FRAME_PRESSURE_MSB_BYTE	    (0)
#define	BM2_DATA_FRAME_PRESSURE_LSB_BYTE		(1)
#define	BM2_DATA_FRAME_PRESSURE_XLSB_BYTE	    (2)
#define	BM2_DATA_FRAME_TEMPERATURE_MSB_BYTE	    (3)
#define	BM2_DATA_FRAME_TEMPERATURE_LSB_BYTE	    (4)
#define	BM2_DATA_FRAME_TEMPERATURE_XLSB_BYTE	(5)
#define	BM2_DATA_FRAME_HUMIDITY_MSB_BYTE		(6)
#define	BM2_DATA_FRAME_HUMIDITY_LSB_BYTE		(7)

#define	BM2_MASK_DIG_H4             (0x0F)

#define BMP280_CHIP_ID              (0x58)
#define BME280_CHIP_ID              (0x60)

#define BM2_I2C_ADDRESS1            (0x76)
#define BM2_I2C_ADDRESS2            (0x77)

#define BM2_SLEEP_MODE              (0x00)
#define BM2_FORCED_MODE             (0x01)
#define BM2_NORMAL_MODE             (0x03)

#define BM2_STANDBY_TIME_1_MS       (0x00)
#define BM2_STANDBY_TIME_63_MS      (0x01)
#define BM2_STANDBY_TIME_125_MS     (0x02)
#define BM2_STANDBY_TIME_250_MS     (0x03)
#define BM2_STANDBY_TIME_500_MS     (0x04)
#define BM2_STANDBY_TIME_1000_MS    (0x05)
#define BM2_STANDBY_TIME_10_MS      (0x06)
#define BM2_STANDBY_TIME_20_MS      (0x07)

#define BM2_OVERSAMP_SKIPPED        (0x00)
#define BM2_OVERSAMP_1X             (0x01)
#define BM2_OVERSAMP_2X             (0x02)
#define BM2_OVERSAMP_4X             (0x03)
#define BM2_OVERSAMP_8X             (0x04)
#define BM2_OVERSAMP_16X            (0x05)

#define BM2_FILTER_COEFF_OFF        (0x00)
#define BM2_FILTER_COEFF_2          (0x01)
#define BM2_FILTER_COEFF_4          (0x02)
#define BM2_FILTER_COEFF_8          (0x03)
#define BM2_FILTER_COEFF_16         (0x04)

#define BM2_SOFT_RESET_CODE             (0xB6)
#define BM2_SOFT_RESET_WAIT_TIME_MS     (3)
#define BM2_STANDARD_OVERSAMP_HUMIDITY	(BM2_OVERSAMP_1X)
#define BM2_INIT_VALUE                  (0)
#define BM2_CHIP_ID_READ_COUNT          (5)
#define BM2_INVALID_DATA                (0)


#define T_INIT_MAX                  (20)
#define T_MEASURE_PER_OSRS_MAX      (37)
#define T_SETUP_PRESSURE_MAX        (10)
#define T_SETUP_HUMIDITY_MAX        (10)

#define BM2_TEMPERATURE_DATA_SIZE   (3)
#define BM2_PRESSURE_DATA_SIZE      (3)
#define BM2_HUMIDITY_DATA_SIZE      (2)
#define BM2_DATA_FRAME_SIZE         (BM2_TEMPERATURE_DATA_SIZE + BM2_PRESSURE_DATA_SIZE + BM2_HUMIDITY_DATA_SIZE)

#define BM2_CALIBRATION_REG_BASE (0x88)

// BME280 register map:

#define BM2_DIG_T1_LSB_REG          (0x88)
#define BM2_DIG_T1_MSB_REG          (0x89)
#define BM2_DIG_T2_LSB_REG          (0x8A)
#define BM2_DIG_T2_MSB_REG          (0x8B)
#define BM2_DIG_T3_LSB_REG          (0x8C)
#define BM2_DIG_T3_MSB_REG          (0x8D)
#define BM2_DIG_P1_LSB_REG          (0x8E)
#define BM2_DIG_P1_MSB_REG          (0x8F)
#define BM2_DIG_P2_LSB_REG          (0x90)
#define BM2_DIG_P2_MSB_REG          (0x91)
#define BM2_DIG_P3_LSB_REG          (0x92)
#define BM2_DIG_P3_MSB_REG          (0x93)
#define BM2_DIG_P4_LSB_REG          (0x94)
#define BM2_DIG_P4_MSB_REG          (0x95)
#define BM2_DIG_P5_LSB_REG          (0x96)
#define BM2_DIG_P5_MSB_REG          (0x97)
#define BM2_DIG_P6_LSB_REG          (0x98)
#define BM2_DIG_P6_MSB_REG          (0x99)
#define BM2_DIG_P7_LSB_REG          (0x9A)
#define BM2_DIG_P7_MSB_REG          (0x9B)
#define BM2_DIG_P8_LSB_REG          (0x9C)
#define BM2_DIG_P8_MSB_REG          (0x9D)
#define BM2_DIG_P9_LSB_REG          (0x9E)
#define BM2_DIG_P9_MSB_REG          (0x9F)
#define BM2_DIG_H1_REG              (0xA1)
#define BM2_DIG_H2_LSB_REG          (0xE1)
#define BM2_DIG_H2_MSB_REG          (0xE2)
#define BM2_DIG_H3_REG              (0xE3)
#define BM2_DIG_H4_MSB_REG          (0xE4)
#define BM2_DIG_H4_LSB_REG          (0xE5)
#define BM2_DIG_H5_MSB_REG          (0xE6)
#define BM2_DIG_H6_REG              (0xE7)
#define BM2_CHIP_ID_REG             (0xD0)
#define BM2_SOFT_RESET_REG          (0xE0)
#define BM2_STATUS_REG              (0xF3) // Status Register
#define BM2_CTRL_MEAS_REG           (0xF4) // Ctrl Measure Register
#define BM2_CTRL_HUMIDITY_REG       (0xF2) // Ctrl Humidity Register
#define BM2_CONFIG_REG              (0xF5) // Configuration Register
#define BM2_PRESSURE_MSB_REG        (0xF7) // Pressure MSB Register
#define BM2_PRESSURE_LSB_REG        (0xF8) // Pressure LSB Register
#define BM2_PRESSURE_XLSB_REG       (0xF9) // Pressure XLSB Register
#define BM2_TEMPERATURE_MSB_REG     (0xFA) // Temperature MSB Reg
#define BM2_TEMPERATURE_LSB_REG     (0xFB) // Temperature LSB Reg
#define BM2_TEMPERATURE_XLSB_REG    (0xFC) // Temperature XLSB Reg
#define BM2_HUMIDITY_MSB_REG        (0xFD) // Humidity MSB Reg
#define BM2_HUMIDITY_LSB_REG        (0xFE) // Humidity LSB Reg

#define BM2_DIG_T1_LSB_POS  (0)
#define BM2_DIG_T1_MSB_POS  (1)
#define BM2_DIG_T2_LSB_POS  (2)
#define BM2_DIG_T2_MSB_POS  (3)
#define BM2_DIG_T3_LSB_POS  (4)
#define BM2_DIG_T3_MSB_POS  (5)
#define BM2_DIG_P1_LSB_POS  (6)
#define BM2_DIG_P1_MSB_POS  (7)
#define BM2_DIG_P2_LSB_POS  (8)
#define BM2_DIG_P2_MSB_POS  (9)
#define BM2_DIG_P3_LSB_POS  (10)
#define BM2_DIG_P3_MSB_POS  (11)
#define BM2_DIG_P4_LSB_POS  (12)
#define BM2_DIG_P4_MSB_POS  (13)
#define BM2_DIG_P5_LSB_POS  (14)
#define BM2_DIG_P5_MSB_POS  (15)
#define BM2_DIG_P6_LSB_POS  (16)
#define BM2_DIG_P6_MSB_POS  (17)
#define BM2_DIG_P7_LSB_POS  (18)
#define BM2_DIG_P7_MSB_POS  (19)
#define BM2_DIG_P8_LSB_POS  (20)
#define BM2_DIG_P8_MSB_POS  (21)
#define BM2_DIG_P9_LSB_POS  (22)
#define BM2_DIG_P9_MSB_POS  (23)
#define BM2_DIG_P10_POS     (24)
#define	BM2_DIG_H1          (25)

#define	BM2_DIG_H2_LSB	    (0)
#define	BM2_DIG_H2_MSB	    (1)
#define	BM2_DIG_H3		    (2)
#define	BM2_DIG_H4_MSB	    (3)
#define	BM2_DIG_H4_LSB	    (4)
#define	BM2_DIG_H5_MSB	    (5)
#define	BM2_DIG_H6		    (6)

class BM2CalData {
    public:
        uint16_t dig_T1  = 0; // 0x88, 0x89
        int16_t  dig_T2  = 0; // 0x8A, 0x8B
        int16_t  dig_T3  = 0; // 0x8C, 0x8D
        
        uint16_t dig_P1  = 0; // 0x8E, 0x8F
        int16_t  dig_P2  = 0; // 0x90, 0x91
        int16_t  dig_P3  = 0; // 0x92, 0x93
        int16_t  dig_P4  = 0; // 0x94, 0x95
        int16_t  dig_P5  = 0; // 0x96, 0x97
        int16_t  dig_P6  = 0; // 0x98, 0x99
        int16_t  dig_P7  = 0; // 0x9A, 0x9B
        int16_t  dig_P8  = 0; // 0x9C, 0x9D
        int16_t  dig_P9  = 0; // 0x9E, 0x9F
        int8_t   dig_P10 = 0; // 0xA0 (nur BMP2x)

        uint8_t  dig_H1  = 0; // 0xA1
        int16_t  dig_H2  = 0; // 0xE1, 0xE2
        uint8_t  dig_H3  = 0; // 0xE3
        int16_t  dig_H4  = 0; // 0xE4, 0xE5[3:0]
        int16_t  dig_H5  = 0; // 0xE5[7:4], 0xE6
        int8_t   dig_H6  = 0; // 0xE7
        
        int32_t  t_fine  = 0; // 4 bytes
};

class SensorBM2x : public SensorDriver {
    private:
        static const SensorProperty bmp280_properties[];
        static const SensorProperty bme280_properties[];

        const char* headName = BMX280_DEVICE_NAME;

        uint8_t chipMemory[256]{ 0 };
        uint8_t chipID = 0xff;
        BM2CalData cal_param;

        int32_t rawPressure    = 0;
        int32_t rawTemperature = 0;
        int32_t rawHumidity    = 0;
        float pressure_hPa     = 0.0f;
        float temperature_C    = 0.0f;
        float humidity_RH      = 0.0f;
        float dewpoint_C       = 0.0f;
        float humidity_AH      = 0.0f;

        esp_err_t init(void);
        esp_err_t cleanup(void);

        float bm2_compensate_temperature(int32_t v_uncom_temperature_s32);
        float bm2_compensate_pressure(int32_t v_uncom_pressure_s32);
        float bme280_compensate_humidity(int32_t v_uncom_humidity_s32);
        void  parse_raw_data(uint8_t* i2c_buffer, int32_t& pressure, int32_t& temperature, int32_t& humidity);

        bool identify_chip(void);
        void reset(void);
        uint32_t generate_serial_number_code(void);
        esp_err_t read_chip_memory(void);
        esp_err_t parse_calibration_data(void);

        esp_err_t enable(void) override;
        esp_err_t disable(void) override;
        esp_err_t fetch(void) override;
        esp_err_t update(void) override;
        
    public:

        SensorBM2x() {
            init();
        }

        ~SensorBM2x() override {
            cleanup();
        }

        const SensorProperty* get_properties(void) override;
        size_t get_property_count(void) override;
        esp_err_t set_heating(bool enable) override;
        const char* get_head(void) override;
        SensorType get_sensor_type(void) override;
        uint8_t get_sub_type(void) override;
};
