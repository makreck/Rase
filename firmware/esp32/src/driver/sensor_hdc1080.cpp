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

const SensorProperty SensorHDC1080::hdc1080_properties[] = {
    { RK_RH, "Relative Humidity",    "RH", "%",      0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#008a7e", 1001 },
    { RK_T,  "Temperature",          "T ", "°C",   -40.0f, 125.0f, SP_FLAGS(1, 2, true,  false), "#ff0000", 1002 },
    { RK_DP, "Dewpoint Temperature", "DP", "°C",   -40.0f, 100.0f, SP_FLAGS(1, 2, true,  false), "#0066ff", 1003 },
    { RK_AH, "Absolute Humidity",    "AH", "g/m³",   0.0f, 580.0f, SP_FLAGS(1, 2, false, false), "#d400ff", 1004 },
};

const SensorProperty* SensorHDC1080::get_properties(void) {
    return (SensorHDC1080::hdc1080_properties);
}

size_t SensorHDC1080::get_property_count(void) {
    return (SIZEOFARRAY(SensorHDC1080::hdc1080_properties));
}

esp_err_t SensorHDC1080::init(void) {
	device_address = HDC1080_DEVICE_ADDRESS;
    reading.setup(get_properties(), get_property_count());
    xTaskCreate(SensorHDC1080::_driverTask, "SensorHDC1080", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY - 1, &task_handle);
    return (ESP_OK);
}

esp_err_t SensorHDC1080::cleanup(void) {
    vTaskDelete(task_handle);
    return (ESP_OK);
}

esp_err_t SensorHDC1080::enable(void) {
    if (swI2CPort.detect(device_address, I2C_MAX_RETRY) != ESP_OK) {
        return (ESP_FAIL);
    }

    esp_err_t result = reset_device();
    if (result == ESP_OK) {
        result = read_manufacturer_id(manufacturerID);
        if (result == ESP_OK) {
            result = read_device_id(deviceID);
            if (result == ESP_OK) {
                result = read_serial_number(head_serial_number);
                if (result == ESP_OK) {
                    if ((deviceID != HDC1080_DEVICE_ID) || (manufacturerID != HDC1080_MANUF_ID_TI)) {
                        result = ESP_FAIL;
                    } else {
                        initialized = true;
                    }
                }
            }
        }
    }

    return (result);
}

esp_err_t SensorHDC1080::disable(void) {
    initialized = false;
    swI2CPort.disable();
    return (ESP_OK);
}

esp_err_t SensorHDC1080::read_config_status(uint16_t& dataWord) {
    uint8_t i2c_buffer[2]{ 0 };
    esp_err_t result = swI2CPort.transact8(device_address, HDC1080_REG_CONFIG_STATUS, i2c_buffer, SIZEOFARRAY(i2c_buffer));
    if (result == ESP_OK) {
        dataWord = BTOW(i2c_buffer);
    }
    return (result);
}

esp_err_t SensorHDC1080::write_config_status(const uint16_t dataWord) {
    uint8_t cmd[3]{ HDC1080_REG_CONFIG_STATUS, MSB(dataWord), LSB(dataWord) };
    return (swI2CPort.send_command_sequence(device_address, cmd, SIZEOFARRAY(cmd)));
}

esp_err_t SensorHDC1080::set_heating(bool enable) {
    uint16_t dataWord = 0;
    if (read_config_status(dataWord) == ESP_OK) {
        dataWord |= ((enable) ? HDC1080_CFR_HEAT_ON : HDC1080_CFR_HEAT_OFF);
        return (write_config_status(dataWord));
    }
    return (ESP_FAIL);
}

esp_err_t SensorHDC1080::reset_device(void) {
    uint16_t dataWord = HDC1080_CFR_DEFAULT;
    read_config_status(dataWord);
    esp_err_t result = write_config_status(dataWord | HDC1080_CFR_RST);
    if (result == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(HDC1080_RESET_TIME_MS));
    }
    return (result);
}

esp_err_t SensorHDC1080::read_temperature(uint16_t& rawTemp) {
    uint8_t i2c_buffer[2]{ 0 };
    esp_err_t result = swI2CPort.transact8(device_address, HDC1080_REG_TEMPERATURE, i2c_buffer, sizeof (i2c_buffer), HDC1080_CONVERSION_TIME_MS);
    rawTemp = BTOW(i2c_buffer);
    return (result);
}

esp_err_t SensorHDC1080::read_humidity(uint16_t& rawRH) {
    uint8_t i2c_buffer[2]{ 0 };
    esp_err_t result = swI2CPort.transact8(device_address, HDC1080_REG_HUMIDITY, i2c_buffer, sizeof (i2c_buffer), HDC1080_CONVERSION_TIME_MS);
    rawRH = BTOW(i2c_buffer);
    return (result);
}

esp_err_t SensorHDC1080::read_device_id(uint16_t& deviceID) {
    uint8_t i2c_buffer[2]{ 0 };
    esp_err_t result = swI2CPort.transact8(device_address, HDC1080_REG_DEVICE_ID, i2c_buffer, sizeof (i2c_buffer));
    deviceID = BTOW(i2c_buffer);
    return (result);
}

esp_err_t SensorHDC1080::read_manufacturer_id(uint16_t& manufacturerID) {
    uint8_t i2c_buffer[2]{ 0 };
    esp_err_t result = swI2CPort.transact8(device_address, HDC1080_REG_MANUFACTURER_ID, i2c_buffer, SIZEOFARRAY(i2c_buffer));
    manufacturerID = BTOW(i2c_buffer);
    return (result);
}

esp_err_t SensorHDC1080::read_serial_number(uint32_t &serialNumberCode) {
    uint8_t i2c_buffer[8]{0};
    esp_err_t result = swI2CPort.transact8(device_address, HDC1080_REG_SERIAL_ID_A, &i2c_buffer[0], 2);
    if (result == ESP_OK) {
        result = swI2CPort.transact8(device_address, HDC1080_REG_SERIAL_ID_B, &i2c_buffer[2], 2);
        if (result == ESP_OK) {
            serialNumberCode = BTOL(i2c_buffer);
            return (((serialNumberCode != 0) && (serialNumberCode != 0xffffffff)) ? ESP_OK : ESP_FAIL);
        }
    }
    return (result);
}

esp_err_t SensorHDC1080::read_serial_number48(uint64_t& serialNumberCode) {
    uint8_t i2c_buffer[8]{ 0 };
    esp_err_t result = swI2CPort.transact8(device_address, HDC1080_REG_SERIAL_ID_A, &i2c_buffer[0], 2);
    if (result == ESP_OK) {
        result = swI2CPort.transact8(device_address, HDC1080_REG_SERIAL_ID_B, &i2c_buffer[2], 2);
        if (result == ESP_OK) {
            result = swI2CPort.transact8(device_address, HDC1080_REG_SERIAL_ID_C, &i2c_buffer[4], 2);
            if (result == ESP_OK) {
                serialNumberCode = ((uint64_t)i2c_buffer[0] << 40)
                                | ((uint64_t)i2c_buffer[1] << 32)
                                | ((uint64_t)i2c_buffer[2] << 24)
                                | ((uint64_t)i2c_buffer[3] << 16)
                                | ((uint64_t)i2c_buffer[4] <<  8)
                                | ((uint64_t)i2c_buffer[5] <<  0);
                result = (((serialNumberCode != 0) && (serialNumberCode != 0x0000ffffffffffff)) ? ESP_OK : ESP_FAIL);
            }
        }
    }
    return (result);
}

esp_err_t SensorHDC1080::read_serial_number_code(uint32_t& serialNoCode) {
    return (read_serial_number(serialNoCode));
}

uint8_t SensorHDC1080::get_sub_type(void) {
    return (0);
}

const char* SensorHDC1080::get_head(void) {
    return (headName);
}

esp_err_t SensorHDC1080::update(void) {
    reading.set_Value(RK_T,  temperature_C);
    reading.set_Value(RK_RH, humidity_RH);
    reading.set_Value(RK_DP, dewpoint_C);
    reading.set_Value(RK_AH, humidity_AH);
    return (ESP_OK);
}

esp_err_t SensorHDC1080::fetch(void) {
    if (read_config_status(dataWord) == ESP_OK) {
        if (read_temperature(rawTemp) == ESP_OK) {
            if (read_humidity(rawRH) == ESP_OK) {
                humidity_RH = ((float)rawRH / 65536.0f) * 100.0f;
                temperature_C = ((float)rawTemp / 65536.0f) * 165.0f - 40.0f;
                apply_magnus_formula(temperature_C, humidity_RH, dewpoint_C, humidity_AH);

#ifdef DISPLAY_STATE
    ESP_LOGI(TAG,
        "%s reading: T=%.1f°C (0x%-4.4X) RH=%.1f%% (0x%-4.4X) DP=%.1f, AH=%.1f",
        get_head(), temperature_C, (unsigned int)rawTemp, humidity_RH, (unsigned int)rawRH, dewpoint_C, humidity_AH);
#endif
                return (ESP_OK);
            }
        }
    }
    
    return (ESP_FAIL);
}

SensorType SensorHDC1080::get_sensor_type() {
    return (SensorType::HDC1080);
}

bool SensorHDC1080::is_connected(SwI2CBus& bus) {
    bus.enable();
    uint8_t i2c_buffer[2]{ 0xff };
    esp_err_t result = bus.transact8(MULTI_ADDR_HDC1080_HTU21_SHT2X, HDC1080_REG_MANUFACTURER_ID, i2c_buffer, SIZEOFARRAY(i2c_buffer));
    uint16_t manufacturerID = BTOW(i2c_buffer);
    bus.disable();
    return ((result == ESP_OK) && (manufacturerID == HDC1080_MANUF_ID_TI));
}
