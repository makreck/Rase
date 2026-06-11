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

//#define DISPLAY_STATE

const SensorProperty SensorBM2x::bme280_properties[] = {
    { RK_AP, "Air Pressure",         "AP", "hPa", 300.0f, 1100.0f, SP_FLAGS(0, 2, false, false), "#ffae00", 1001 },
    { RK_RH, "Relative Humidity",    "RH", "%",      0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#008a7e", 1002 },
    { RK_T,  "Temperature",          "T ", "°C",   -40.0f,  85.0f, SP_FLAGS(1, 2, true,  false), "#ff0000", 1003 },
    { RK_DP, "Dewpoint Temperature", "DP", "°C",   -40.0f,  85.0f, SP_FLAGS(1, 2, true,  false), "#0066ff", 1004 },
    { RK_AH, "Absolute Humidity",    "AH", "g/m³",   0.0f, 580.0f, SP_FLAGS(1, 2, false, false), "#d400ff", 1005 },
};

const SensorProperty SensorBM2x::bmp280_properties[] = {
    { RK_AP, "Air Pressure",         "AP", "hPa", 300.0f, 1100.0f, SP_FLAGS(0, 2, false, false), "#ffae00", 1001 },
    { RK_T,  "Temperature",          "T ", "°C",   -40.0f,  85.0f, SP_FLAGS(1, 2, true,  false), "#ff0000", 1003 },
};

const SensorProperty* SensorBM2x::get_properties(void) {
    if (chipID == BMP280_CHIP_ID) {
        return (SensorBM2x::bmp280_properties);
    } else {
        return (SensorBM2x::bme280_properties);
    }
}

size_t SensorBM2x::get_property_count(void) {
    if (chipID == BME280_CHIP_ID) {
        return (SIZEOFARRAY(SensorBM2x::bme280_properties));
    } else {
        return (SIZEOFARRAY(SensorBM2x::bmp280_properties));
    }
}

const uint8_t cmdSetup[] = {
    BM2_CTRL_HUMIDITY_REG, BM2_OVERSAMP_4X,
    BM2_CTRL_MEAS_REG,     (BM2_NORMAL_MODE | (BM2_OVERSAMP_16X << 2) | (BM2_OVERSAMP_16X << 5)),
};

esp_err_t SensorBM2x::init(void) {
	device_address = MULTI_ADDR_BMP280_BME280;
    xTaskCreate(SensorBM2x::_driverTask, "SensorBM2x", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY - 1, &task_handle);
    return (ESP_OK);
}

esp_err_t SensorBM2x::cleanup(void) {
    vTaskDelete(task_handle);
    disable();
    return (ESP_OK);
}

esp_err_t SensorBM2x::enable(void) {
    if (swI2CPort.detect(device_address, I2C_MAX_RETRY) != ESP_OK) {
        return (ESP_FAIL);
    }

    if (!identify_chip()) {
        return (ESP_FAIL);
    }

    reset();

    if (read_chip_memory() != ESP_OK) {
        return (ESP_FAIL);
    }

    parse_calibration_data();
    head_serial_number = generate_serial_number_code();
    esp_err_t result = swI2CPort.write_register(device_address, BM2_CONFIG_REG, (BM2_STANDBY_TIME_1_MS << 5) | (BM2_FILTER_COEFF_2 << 2));
    if (result == ESP_OK) {
        result = swI2CPort.send_command_sequence(device_address, cmdSetup, SIZEOFARRAY(cmdSetup));
    }

    initialized = (result == ESP_OK);
    return (result);
}

esp_err_t SensorBM2x::disable(void) {
    swI2CPort.disable();
    initialized = false;
    return (ESP_OK);
}

uint32_t SensorBM2x::generate_serial_number_code(void) {
    // A virtual serial number code is generated from the chip specific calibration data!
    uint32_t code = 0;
    uint32_t* p = (uint32_t*)(&chipMemory[BM2_CALIBRATION_REG_BASE]);
    code = p[0];
    for (int i = 1; i < 6; i++) {
        code ^= p[i];
    }
    return (code);
}

esp_err_t SensorBM2x::read_chip_memory(void) {
    memset(chipMemory, 0xff, sizeof (chipMemory));
    swI2CPort.transact8(device_address, BM2_CALIBRATION_REG_BASE, &chipMemory[BM2_CALIBRATION_REG_BASE], (size_t)(0x100 - BM2_CALIBRATION_REG_BASE));

    esp_err_t result = ESP_FAIL;
    for (int i = 0x88; i < 256; i += 8) {
        if (chipMemory[i] != 0xff) {
            result = ESP_OK;
        }
    }
    return (result);
}

esp_err_t SensorBM2x::parse_calibration_data(void) {
    uint8_t* p = &chipMemory[BM2_CALIBRATION_REG_BASE];

    cal_param.dig_T1  = (uint16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_T1_MSB_POS], p[BM2_DIG_T1_LSB_POS]));
    cal_param.dig_T2  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_T2_MSB_POS], p[BM2_DIG_T2_LSB_POS]));
    cal_param.dig_T3  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_T3_MSB_POS], p[BM2_DIG_T3_LSB_POS]));
    
    cal_param.dig_P1  = (uint16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P1_MSB_POS], p[BM2_DIG_P1_LSB_POS]));
    cal_param.dig_P2  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P2_MSB_POS], p[BM2_DIG_P2_LSB_POS]));
    cal_param.dig_P3  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P3_MSB_POS], p[BM2_DIG_P3_LSB_POS]));
    cal_param.dig_P4  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P4_MSB_POS], p[BM2_DIG_P4_LSB_POS]));
    cal_param.dig_P5  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P5_MSB_POS], p[BM2_DIG_P5_LSB_POS]));
    cal_param.dig_P6  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P6_MSB_POS], p[BM2_DIG_P6_LSB_POS]));
    cal_param.dig_P7  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P7_MSB_POS], p[BM2_DIG_P7_LSB_POS]));
    cal_param.dig_P8  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P8_MSB_POS], p[BM2_DIG_P8_LSB_POS]));
    cal_param.dig_P9  = (int16_t)(BM2_MSBLSB_TO_U16(p[BM2_DIG_P9_MSB_POS], p[BM2_DIG_P9_LSB_POS]));
    cal_param.dig_P10 = (int8_t)((uint8_t)(p[BM2_DIG_P10_POS]));
    cal_param.dig_H1  = (uint8_t)p[BM2_DIG_H1];

    if (chipID == BME280_CHIP_ID) {
        p = &chipMemory[BM2_DIG_H2_LSB_REG];
        cal_param.dig_H2 = (int16_t)((((int16_t)((int8_t)p[BM2_DIG_H2_MSB])) << 8) | p[BM2_DIG_H2_LSB]);
        cal_param.dig_H3 = p[BM2_DIG_H3];
        cal_param.dig_H4 = (int16_t)((((int16_t)((int8_t)p[BM2_DIG_H4_MSB])) << 4) | (((uint8_t)BM2_MASK_DIG_H4) & p[BM2_DIG_H4_LSB]));
        cal_param.dig_H5 = (int16_t)((((int16_t)((int8_t)p[BM2_DIG_H5_MSB])) << 4) | (p[BM2_DIG_H4_LSB] >> 4));
        cal_param.dig_H6 = (int8_t)p[BM2_DIG_H6];
    }

    return (ESP_OK);
}

void SensorBM2x::reset(void) {
    swI2CPort.write_register(device_address, BM2_CTRL_MEAS_REG, BM2_SLEEP_MODE);
    vTaskDelay(pdMS_TO_TICKS(BM2_SOFT_RESET_WAIT_TIME_MS));
    swI2CPort.write_register(device_address, BM2_SOFT_RESET_REG, BM2_SOFT_RESET_CODE);
    vTaskDelay(pdMS_TO_TICKS(BM2_SOFT_RESET_WAIT_TIME_MS));
}

bool SensorBM2x::identify_chip(void) {
    headName = "BMx2";
    if (swI2CPort.read_register(device_address, BM2_CHIP_ID_REG, chipID) == ESP_OK) {
        if (chipID == BMP280_CHIP_ID) {
            headName = "BMP280";
            return (true);
        } else if (chipID == BME280_CHIP_ID) {
            headName = "BME280";
            return (true);
        }
    }
    return (false);
}

void SensorBM2x::parse_raw_data(uint8_t* i2c_buffer, int32_t& pressure, int32_t& temperature, int32_t& humidity) {
    uint32_t data_msb;
    uint32_t data_lsb;
    uint32_t data_xlsb;

    data_msb = (uint32_t)i2c_buffer[BM2_DATA_FRAME_PRESSURE_MSB_BYTE] << 12;
    data_lsb = (uint32_t)i2c_buffer[BM2_DATA_FRAME_PRESSURE_LSB_BYTE] << 4;
    data_xlsb = (uint32_t)i2c_buffer[BM2_DATA_FRAME_PRESSURE_XLSB_BYTE] >> 4;
    pressure = (int32_t)(data_msb | data_lsb | data_xlsb);

    data_msb = (int32_t)i2c_buffer[BM2_DATA_FRAME_TEMPERATURE_MSB_BYTE] << 12;
    data_lsb = (int32_t)i2c_buffer[BM2_DATA_FRAME_TEMPERATURE_LSB_BYTE] << 4;
    data_xlsb = (int32_t)i2c_buffer[BM2_DATA_FRAME_TEMPERATURE_XLSB_BYTE] >> 4;
    temperature = (int32_t)(data_msb | data_lsb | data_xlsb);

    humidity = 0;
    if (chipID == BME280_CHIP_ID) {
        data_msb = ((uint32_t)i2c_buffer[BM2_DATA_FRAME_HUMIDITY_MSB_BYTE]) << 8;
        data_lsb = ((uint32_t)i2c_buffer[BM2_DATA_FRAME_HUMIDITY_LSB_BYTE]);
        humidity = (int32_t)(data_msb | data_lsb);
    }
}

float SensorBM2x::bm2_compensate_temperature(int32_t temp) {
    double var1 = (((double)temp) / 16384.0 - ((double) cal_param.dig_T1) / 1024.0) * ((double) cal_param.dig_T2);

    double var2 = ((((double)temp) / 131072.0 - ((double) cal_param.dig_T1) / 8192.0) *
           (((double)temp) / 131072.0 - ((double) cal_param.dig_T1) / 8192.0)) * ((double) cal_param.dig_T3);

    cal_param.t_fine = (int32_t) (var1 + var2);

    return ((float)((var1 + var2) / 5120.0));
}

float SensorBM2x::bm2_compensate_pressure(int32_t v_uncom_pressure_s32) {
    double v_x1_u32 = ((double)cal_param.t_fine / 2.0) - 64000.0;
    double v_x2_u32 = v_x1_u32 * v_x1_u32 * ((double)cal_param.dig_P6) / 32768.0;
    v_x2_u32 = v_x2_u32 + v_x1_u32 * ((double)cal_param.dig_P5) * 2.0;
    v_x2_u32 = (v_x2_u32 / 4.0) + (((double)cal_param.dig_P4) * 65536.0); 
    v_x1_u32 = (((double)cal_param.dig_P3) * v_x1_u32 * v_x1_u32 / 524288.0 + ((double)cal_param.dig_P2) * v_x1_u32) / 524288.0;
    v_x1_u32 = (1.0 + v_x1_u32 / 32768.0) * ((double)cal_param.dig_P1);
    
    if (v_x1_u32 == 0) {
        return (BM2_INVALID_DATA);
    }

    double pressure = 1048576.0 - (double)v_uncom_pressure_s32;
    pressure = (pressure - (v_x2_u32 / 4096.0)) * 6250.0 / v_x1_u32;
    v_x1_u32 = ((double)cal_param.dig_P9) * pressure * pressure / 2147483648.0;
    v_x2_u32 = pressure * ((double)cal_param.dig_P8) / 32768.0;
    pressure = pressure + (v_x1_u32 + v_x2_u32 + ((double)cal_param.dig_P7)) / 16.0;

    return ((float)pressure);
}

float SensorBM2x::bme280_compensate_humidity(int32_t v_uncom_humidity_s32) {
	double var_h = (((double)cal_param.t_fine) - 76800.0);
    var_h = (v_uncom_humidity_s32 - (((double)cal_param.dig_H4) * 64.0 + ((double)cal_param.dig_H5) / 16384.0 * var_h)) *
            (((double)cal_param.dig_H2) / 65536.0 * (1.0 + ((double)cal_param.dig_H6) / 67108864.0 * var_h * (1.0 + ((double)cal_param.dig_H3) / 67108864.0 * var_h)));
    return ((float)((var_h * (1.0 - ((double)cal_param.dig_H1) * var_h / 524288.0))));
}

uint8_t SensorBM2x::get_sub_type(void) {
    return (chipID);
}

esp_err_t SensorBM2x::set_heating(bool enable) {
	return (ESP_FAIL);
}

const char* SensorBM2x::get_head(void) {
    return (headName);
}

esp_err_t SensorBM2x::update(void) {
    reading.set_Value(RK_AP, pressure_hPa);
    reading.set_Value(RK_T,  temperature_C);

    if (chipID == BME280_CHIP_ID) {
        reading.set_Value(RK_RH, humidity_RH);
        reading.set_Value(RK_DP, dewpoint_C);
        reading.set_Value(RK_AH, humidity_AH);
    }

    return (ESP_OK);
}

esp_err_t SensorBM2x::fetch(void) {
    uint8_t i2c_buffer[BM2_ALL_DATA_FRAME_LENGTH]{0};
    esp_err_t result = swI2CPort.transact8(device_address, BM2_PRESSURE_MSB_REG, i2c_buffer, BM2_ALL_DATA_FRAME_LENGTH);
    if (result == ESP_OK) {
        parse_raw_data(i2c_buffer, rawPressure, rawTemperature, rawHumidity);
        temperature_C = bm2_compensate_temperature(rawTemperature);
        pressure_hPa = bm2_compensate_pressure(rawPressure) / 100.0;
        if (chipID == BME280_CHIP_ID) {
            humidity_RH = bme280_compensate_humidity(rawHumidity);
            apply_magnus_formula(temperature_C, humidity_RH, dewpoint_C, humidity_AH);
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG,
                "%s reading: P=%.1fhPa (0x%-4.4X) T=%.1f°C (0x%-4.4X) RH=%.1f%% (0x%-4.4X) DP=%.1f, AH=%.1f",
                get_head(), 
                pressure_hPa, (unsigned int)rawPressure,
                temperature_C, (unsigned int)rawTemperature, 
                humidity_RH, (unsigned int)rawHumidity,
                dewpoint_C, humidity_AH);
#endif
            return (ESP_OK);
        } else if (chipID == BMP280_CHIP_ID) {
            humidity_RH = -1.0f;
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG,
                "%s reading: P=%.1fhPa (0x%-4.4X) T=%.1f°C (0x%-4.4X)",
                get_head(), 
                pressure_hPa, (unsigned int)rawPressure,
                temperature_C, (unsigned int)rawTemperature);
#endif
            return (ESP_OK);
        }
    }
    
    return (ESP_FAIL);
}

SensorType SensorBM2x::get_sensor_type() {
    return (SensorType::BMx280);
}
