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

const SensorProperty SensorSHT2::sht2_properties[] = {
    { RK_RH, "Relative Humidity",    "RH", "%",      0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#008a7e", 1001 },
    { RK_T,  "Temperature",          "T ", "°C",   -40.0f, 125.0f, SP_FLAGS(1, 2, true,  false), "#ff0000", 1002 },
    { RK_DP, "Dewpoint Temperature", "DP", "°C",   -40.0f, 100.0f, SP_FLAGS(1, 2, true,  false), "#0066ff", 1003 },
    { RK_AH, "Absolute Humidity",    "AH", "g/m³",   0.0f, 580.0f, SP_FLAGS(1, 2, false, false), "#d400ff", 1004 },
};

const SensorProperty* SensorSHT2::get_properties(void) {
    return (SensorSHT2::sht2_properties);
}

size_t SensorSHT2::get_property_count(void) {
    return (SIZEOFARRAY(SensorSHT2::sht2_properties));
}

esp_err_t SensorSHT2::init(void) {
    device_address = SHT2_DEVICE_ADDRESS;
    reading.setup(get_properties(), get_property_count());
    xTaskCreate(SensorSHT2::_driverTask, "SensorSHT2", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY - 1, &task_handle);
    return (ESP_OK);
}

esp_err_t SensorSHT2::cleanup(void) {
    vTaskDelete(task_handle);
    task_handle = nullptr;
    return (ESP_OK);
}

const char* SensorSHT2::get_head(void) {
    return (headName);
}

esp_err_t SensorSHT2::disable(void) {
    swI2CPort.disable();
    initialized = false;
    return (ESP_OK);
}

esp_err_t SensorSHT2::set_heating(bool enable) {
    return (ESP_FAIL);
}

uint8_t SensorSHT2::get_sub_type(void) {
    return (0);
}

SensorType SensorSHT2::get_sensor_type() {
    return (SensorType::SHT2x);
}

bool SensorSHT2::is_connected(SwI2CBus& bus) {
    bus.enable();
    esp_err_t result = bus.send_command(SHT2_DEVICE_ADDRESS, (uint8_t)SHT2Command::SOFT_RESET);
    if (result == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(SHT2_RESET_TIME_MS));

        // Read User Register with CRC (2 bytes) is only working with the SHT2x and not with the HTU21d.
        // So, we can use this feature to identify the sensor type (SHT2x or HTU21d)!
        uint8_t i2c_buffer[2]{ 0xff };
        result = bus.transact8(SHT2_DEVICE_ADDRESS, (uint8_t)SHT2Command::READ_USER_REGISTER, i2c_buffer, 2, 1);
        if (result == ESP_OK) {
            if (!check_crc(i2c_buffer, 1, i2c_buffer[1])) {
                result = ESP_FAIL;
            }
        }
    }
	bus.disable();
	
    return (result == ESP_OK);
}

esp_err_t SensorSHT2::write_user_register(uint8_t data) {
    return (swI2CPort.write_register(device_address, (uint8_t)SHT2Command::WRITE_USER_REGISTER, data));
}

esp_err_t SensorSHT2::read_user_register(uint8_t* data) {
	uint8_t i2c_buffer[2]{ 0xff };
	esp_err_t result = swI2CPort.transact8(device_address, (uint8_t)SHT2Command::READ_USER_REGISTER, i2c_buffer, 2, 1);
    if (result == ESP_OK) {
		if (check_crc(i2c_buffer, 1, i2c_buffer[1])) {

            user_register = i2c_buffer[0];
            if ((i2c_buffer[0] & SHT2_RES_MASK) == SHT2_RES_RH12_T14) {
                channel.RH.setup(12, SHT2_CONV_TIME_RH_12B_MS, SHT2_MASK_12_BIT);
                channel.T.setup(14, SHT2_CONV_TIME_T_14B_MS, SHT2_MASK_14_BIT);
            } else if ((i2c_buffer[0] & SHT2_RES_MASK) == SHT2_RES_RH08_T12) {
                channel.RH.setup(8, SHT2_CONV_TIME_RH_8B_MS, SHT2_MASK__8_BIT);
                channel.T.setup(12, SHT2_CONV_TIME_T_12B_MS, SHT2_MASK_12_BIT);
            } else if ((i2c_buffer[0] & SHT2_RES_MASK) == SHT2_RES_RH10_T13) {
                channel.RH.setup(10, SHT2_CONV_TIME_RH_10B_MS, SHT2_MASK_10_BIT);
                channel.T.setup(13, SHT2_CONV_TIME_T_13B_MS, SHT2_MASK_13_BIT);
            } else if ((i2c_buffer[0] & SHT2_RES_MASK) == SHT2_RES_RH11_T11) {
                channel.RH.setup(11, SHT2_CONV_TIME_RH_11B_MS, SHT2_MASK_11_BIT);
                channel.T.setup(11, SHT2_CONV_TIME_T_11B_MS, SHT2_MASK_11_BIT);
            }

            disable_OTP_reload = ((i2c_buffer[0] & SHT2_OTP_RELOAD_MASK)     == SHT2_OTP_RELOAD_MASK);
            enable_heater      = ((i2c_buffer[0] & SHT2_HEATER_MASK)         == SHT2_HEATER_MASK);
            end_of_battery     = ((i2c_buffer[0] & SHT2_END_OF_BATTERY_MASK) == SHT2_END_OF_BATTERY_MASK);

            if (data != nullptr) {
                *data = user_register;
            }

            ESP_LOGI(TAG, "SensorSHT2::read_user_register(). Read successful, content = 0x%-2.2X crc=0x%-2.2X, result = %d", (unsigned int)i2c_buffer[0], (unsigned int)i2c_buffer[1], result);
        } else {
            result = ESP_FAIL;
            ESP_LOGE(TAG, "SensorSHT2::read_user_register(). CRC Error, result = %d", result);
        }
    } else {
        ESP_LOGE(TAG, "SensorSHT2::read_user_register(). Error, unable to read User Register content, result = %d", result);
    }

    return (result);
}

esp_err_t SensorSHT2::soft_reset(void) {
    esp_err_t result = swI2CPort.send_command(device_address, (uint8_t)SHT2Command::SOFT_RESET);
    if (result == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(SHT2_RESET_TIME_MS));
    }
    return (result);
}

bool SensorSHT2::check_crc(uint8_t* data, uint8_t length, uint8_t checksum) {
    uint8_t crc = 0;
    for (uint8_t byte_count = 0; byte_count < length; byte_count++) {
        crc ^= data[byte_count];
        for (uint8_t bit = 8; bit > 0; bit--) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ SHT20_POLYNOMIAL;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return (crc == checksum);
}

esp_err_t SensorSHT2::read_measurement(uint8_t cmd, uint32_t wait_ms, uint16_t& raw_value) {
	uint8_t i2c_buffer[4]{0xff};
	esp_err_t result = swI2CPort.transact8(device_address, cmd, i2c_buffer, 3, wait_ms);
	if (result == ESP_OK) {
		if (check_crc(i2c_buffer, 2, i2c_buffer[2])) {
            raw_value = ((uint16_t)i2c_buffer[0] << 8) | (uint16_t)(i2c_buffer[1] & 0xfc);
        } else {
            result = ESP_FAIL;
        }
	}
	return (result);
}

esp_err_t SensorSHT2::read_serial_number_code(SwI2CBus& bus, uint8_t address, uint32_t& serialNumberCode, char* serialNumberString, size_t snLength) {
	uint8_t cmd_data[2]{0};
	uint8_t i2c_buffer[16] { 0xff };

    cmd_data[0] = (uint8_t)SHT2Command::READ_OTP_MEMORY;
	cmd_data[1] = (uint8_t)SHT2Command::READ_OTP_SERIAL_START_ADR;
	esp_err_t result = bus.transact(address, cmd_data, 2, &i2c_buffer[0], 8);

    if (result == ESP_OK) {
        cmd_data[0] = (uint8_t)SHT2Command::READ_METAL_ROM;
        cmd_data[1] = (uint8_t)SHT2Command::READ_ROM_SERIAL_START_ADR;
        result = bus.transact(address, cmd_data, 2, &i2c_buffer[8], 6);

        if (result == ESP_OK) {
            int n = 0;
            for (int i = 0; i < 8; i += 2) {
                if (SensorSHT2::check_crc(&i2c_buffer[i], 1, i2c_buffer[i + 1])) n++;
            }
            for (int i = 8; i < 14; i += 3) {
                if (SensorSHT2::check_crc(&i2c_buffer[i], 2, i2c_buffer[i + 2])) n++;
            }
            if (n != 6) {
                result = ESP_FAIL;
            }
        }
    }

    if (result == ESP_OK) {
        serialNumberCode = (((uint32_t)i2c_buffer[0] << 24) | ((uint32_t)i2c_buffer[2] << 16) | ((uint32_t)i2c_buffer[4]  << 8) | ((uint32_t)i2c_buffer[6]));
        if ((serialNumberString != nullptr) && (snLength >= 15)) {
            memset(serialNumberString, 0, snLength);
            snprintf(serialNumberString, snLength,
                "%-2.2X%-2.2X%-2.2X%-2.2X-%-2.2X%-2.2X%-2.2X", 
                (unsigned int)i2c_buffer[0], (unsigned int)i2c_buffer[2], (unsigned int)i2c_buffer[4], (unsigned int)i2c_buffer[6],
                (unsigned int)i2c_buffer[8], (unsigned int)i2c_buffer[10], (unsigned int)i2c_buffer[12]);
        }
    }

    return (result);
}

esp_err_t SensorSHT2::enable(void) {
    if (swI2CPort.detect(device_address, I2C_MAX_RETRY) != ESP_OK) {
        return (ESP_FAIL);
    }

    esp_err_t result = soft_reset();
    if (result == ESP_OK) {
        result = read_serial_number_code(swI2CPort, device_address,
            head_serial_number, device_serial_number, sizeof (device_serial_number));
        if (result == ESP_OK) {
            result = read_user_register();
            if (result == ESP_OK) {
                initialized = true;
            }
        }
    }

    return (result);
}

esp_err_t SensorSHT2::update(void) {
    reading.set_Value(RK_T,  temperature_C);
    reading.set_Value(RK_RH, humidity_RH);
    reading.set_Value(RK_DP, dewpoint_C);
    reading.set_Value(RK_AH, humidity_AH);
    return (ESP_OK);
}

esp_err_t SensorSHT2::fetch(void) {
    if (!initialized) {
        return (ESP_FAIL);
    }

	if (read_measurement((uint8_t)SHT2Command::TRIGGER_T_NO_HOLD_MASTER, channel.T.conv_time_ms, channel.T.raw_data) == ESP_OK) {
		if (read_measurement((uint8_t)SHT2Command::TRIGGER_RH_NO_HOLD_MASTER, channel.RH.conv_time_ms, channel.RH.raw_data) == ESP_OK) {

            temperature_C = -46.85f + 175.72f * channel.T.get_value();
			humidity_RH   = -6.0f + 125.0f * channel.RH.get_value();

            apply_magnus_formula(temperature_C, humidity_RH, dewpoint_C, humidity_AH);

#ifdef DISPLAY_STATE
			ESP_LOGI(TAG, "SensorSHT2x::fetch(0x%-2.2X).readings sn=\"%s\", "
				"T=%.1f°C (0x%-4.4X) RH=%.1f%% (0x%-4.4X) DP=%.1f, AH=%.1f   ",
                (unsigned int)device_address, device_serial_number, temperature_C,
                (unsigned int)channel.T.get_raw_value(), humidity_RH, (unsigned int)channel.RH.get_raw_value(),
                dewpoint_C, humidity_AH);
#endif
        }
    }

    return (ESP_OK);
}
