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

const SensorProperty SensorSHT3::sht3_properties[] = {
    { RK_RH, "Relative Humidity",    "RH", "%",      0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#008a7e", 1001 },
    { RK_T,  "Temperature",          "T ", "°C",   -45.0f, 130.0f, SP_FLAGS(1, 2, true,  false), "#ff0000", 1002 },
    { RK_DP, "Dewpoint Temperature", "DP", "°C",   -45.0f, 100.0f, SP_FLAGS(1, 2, true,  false), "#0066ff", 1003 },
    { RK_AH, "Absolute Humidity",    "AH", "g/m³",   0.0f, 580.0f, SP_FLAGS(1, 2, false, false), "#d400ff", 1004 },
};

const SensorProperty* SensorSHT3::get_properties(void) {
    return (SensorSHT3::sht3_properties);
}

size_t SensorSHT3::get_property_count(void) {
    return (SIZEOFARRAY(SensorSHT3::sht3_properties));
}

esp_err_t SensorSHT3::init(void) {
    device_address = SHT3_DEVICE_ADDRESS;
    reading.setup(get_properties(), get_property_count());
    xTaskCreate(SensorSHT3::_driverTask, "driverSHT3", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY - 1, &task_handle);
    return (ESP_OK);
}

esp_err_t SensorSHT3::cleanup(void) {
    vTaskDelete(task_handle);
    task_handle = nullptr;
    return (ESP_OK);
}

const char* SensorSHT3::get_head(void) {
    return (headName);
}

esp_err_t SensorSHT3::enable(void) {
    if (swI2CPort.detect(device_address, I2C_MAX_RETRY) != ESP_OK) {
        return (ESP_FAIL);
    }

    status.clear();

    if (clear_status_code() == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(SHT_CLEAR_STATUS_DELAY_MS));
        if (read_serial_number_code(head_serial_number) == ESP_OK) {
            if (set_heating(SHT3Command::SHT3_CMD_HEATER_DISABLE) == ESP_OK) {
                if (start_periodic_measurement(SHT3Command::SHT3_CMD_MEAS_PERI_2_H) == ESP_OK) {
                    if (read_status_code(status) == ESP_OK) {
                        initialized = true;
                        return (ESP_OK);
                    }
                }
            }
        }
    }

    return (ESP_FAIL);
}

esp_err_t SensorSHT3::disable(void) {
	swI2CPort.enable();
    initialized = true;
	return (read_serial_number_code(head_serial_number));
}

float SensorSHT3::convert_raw_data_RH(uint16_t wRawDataRH) {
    return (((float)wRawDataRH / 65535.0f) * 100.0f);
}

float SensorSHT3::convert_raw_data_Temp(uint16_t wRawDataTemp) {
    return (((float)wRawDataTemp / 65535.0f) * 175.0f - 45.0f);
}

esp_err_t SensorSHT3::enable_ART(void) {
    return (swI2CPort.transact16(device_address, (uint16_t)SHT3Command::SHT3_CMD_MEAS_ART, nullptr, 0));
}

esp_err_t SensorSHT3::set_heating(SHT3Command commandCode) {
    if ((commandCode != SHT3Command::SHT3_CMD_HEATER_ENABLE) && (commandCode != SHT3Command::SHT3_CMD_HEATER_DISABLE)) {
        return (ESP_ERR_INVALID_ARG);
    }
    esp_err_t result = swI2CPort.transact16(device_address, (uint16_t)commandCode, nullptr, 0);
    if (result == ESP_OK) {
        result = read_status_code(status);
        if (result == ESP_OK) {
            if (commandCode == SHT3Command::SHT3_CMD_HEATER_ENABLE) {
                result = (status.heaterEnable == 1) ? ESP_OK : ESP_FAIL;
            } else {
                result = (status.heaterEnable == 0) ? ESP_OK : ESP_FAIL;
            }
        }
    }
    return (result);
}

uint8_t SensorSHT3::calc_crc(uint8_t* pData, uint8_t nLen) {
    uint8_t bChkSum = 0xff;
    for (uint8_t n = 0; n < nLen; n++) {
        bChkSum ^= (pData[n]);
        for (uint8_t i = 0; i < 8; i++) {
            if (bChkSum & 0x80) {
                bChkSum = (bChkSum << 1) ^ I2C_POLYNOMIAL;
            } else {
                bChkSum = (bChkSum << 1);
            }
        }
    }
    return (bChkSum);
}

bool SensorSHT3::bi_value_crc_check(uint8_t* buffer) {
    return ((calc_crc(&buffer[0], 2) == buffer[2]) && (calc_crc(&buffer[3], 2) == buffer[5]));
}

esp_err_t SensorSHT3::read_serial_number_code(uint32_t& serialNoCode) {
    serialNoCode = 0;
    uint8_t buffer[6]{ 0 };
    esp_err_t result = swI2CPort.transact16(device_address, (uint16_t)SHT3Command::SHT3_CMD_READ_SERIALNBR, buffer, SIZEOFARRAY(buffer));
    if (result == ESP_OK) {
        if (bi_value_crc_check(buffer)) {
            serialNoCode = B3TOL(buffer);
        } else {
            result = ESP_FAIL;
        }
    }
    return (result);
}

esp_err_t SensorSHT3::read_status_code(STH3State& status) {
    uint8_t buffer[3]{ 0 };
    esp_err_t result = swI2CPort.transact16(device_address, (uint16_t)SHT3Command::SHT3_CMD_READ_STATUS, buffer, SIZEOFARRAY(buffer));
    if (result == ESP_OK) {
        if ((calc_crc(&buffer[0], 2) == buffer[2])) {
            status.set(buffer);
        } else {
            result = ESP_FAIL;
        }
    }
    return (result);
}

esp_err_t SensorSHT3::clear_status_code(void) {
    return (swI2CPort.transact16(device_address, (uint16_t)SHT3Command::SHT3_CMD_CLEAR_STATUS, nullptr, 0));
}

esp_err_t SensorSHT3::start_periodic_measurement(SHT3Command cmdCode) {
    switch (cmdCode) {
        case SHT3Command::SHT3_CMD_MEAS_PERI_05_H:
        case SHT3Command::SHT3_CMD_MEAS_PERI_05_M:
        case SHT3Command::SHT3_CMD_MEAS_PERI_05_L:
        case SHT3Command::SHT3_CMD_MEAS_PERI_1_H:
        case SHT3Command::SHT3_CMD_MEAS_PERI_1_M:
        case SHT3Command::SHT3_CMD_MEAS_PERI_1_L:
        case SHT3Command::SHT3_CMD_MEAS_PERI_2_H:
        case SHT3Command::SHT3_CMD_MEAS_PERI_2_M:
        case SHT3Command::SHT3_CMD_MEAS_PERI_2_L:
        case SHT3Command::SHT3_CMD_MEAS_PERI_4_H:
        case SHT3Command::SHT3_CMD_MEAS_PERI_4_M:
        case SHT3Command::SHT3_CMD_MEAS_PERI_4_L:
        case SHT3Command::SHT3_CMD_MEAS_PERI_10_H:
        case SHT3Command::SHT3_CMD_MEAS_PERI_10_M:
        case SHT3Command::SHT3_CMD_MEAS_PERI_10_L: {
        } break;

        default: {
        } return (ESP_ERR_INVALID_ARG);
    }

    return (swI2CPort.transact16(device_address, (uint16_t)cmdCode, nullptr, 0));
}

esp_err_t SensorSHT3::set_heating(bool enable) {
    return (set_heating((enable) ? SHT3Command::SHT3_CMD_HEATER_ENABLE : SHT3Command::SHT3_CMD_HEATER_DISABLE));
}

uint8_t SensorSHT3::get_sub_type(void) {
    return (0);
}

esp_err_t SensorSHT3::update(void) {
    reading.set_Value(RK_T,  temperature_C);
    reading.set_Value(RK_RH, humidity_RH);
    reading.set_Value(RK_DP, dewpoint_C);
    reading.set_Value(RK_AH, humidity_AH);
    return (ESP_OK);
}

esp_err_t SensorSHT3::fetch(void) {
    uint8_t i2c_buffer[6]{0xff};

    if (swI2CPort.transact16(device_address, (uint16_t)SHT3Command::SHT3_CMD_FETCH_DATA, i2c_buffer, sizeof(i2c_buffer)) == ESP_OK) {
        if (bi_value_crc_check(i2c_buffer)) {
            wRawDataTemp = BTOW(&i2c_buffer[0]);
            wRawDataRH = BTOW(&i2c_buffer[3]);
            temperature_C = convert_raw_data_Temp(wRawDataTemp);
            humidity_RH = convert_raw_data_RH(wRawDataRH);

            apply_magnus_formula(temperature_C, humidity_RH, dewpoint_C, humidity_AH);

#ifdef DISPLAY_STATE
            ESP_LOGI(TAG,
                "%s reading: T=%.1f°C (0x%-4.4X) RH=%.1f%% (0x%-4.4X) DP=%.1f, AH=%.1f",
                get_head(), temperature_C, (unsigned int)wRawDataTemp, humidity_RH, (unsigned int)wRawDataRH, dewpoint_C, humidity_AH);
#endif
            return (ESP_OK);
        }
    }

    return (ESP_FAIL);
}

SensorType SensorSHT3::get_sensor_type() {
    return (SensorType::SHT3x);
}
