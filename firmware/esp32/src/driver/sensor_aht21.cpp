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

const SensorProperty SensorAHTxx::athxx_properties[] = {
    { RK_RH, "Relative Humidity",    "RH", "%",      0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#008a7e", 1001},
    { RK_T,  "Temperature",          "T ", "°C",   -40.0f, 125.0f, SP_FLAGS(1, 2, true,  false), "#ff0000", 1002 },
    { RK_DP, "Dewpoint Temperature", "DP", "°C",   -40.0f, 100.0f, SP_FLAGS(1, 2, true,  false), "#0066ff", 1003 },
    { RK_AH, "Absolute Humidity",    "AH", "g/m³",   0.0f, 580.0f, SP_FLAGS(1, 2, false, false), "#d400ff", 1004 },
};

const SensorProperty* SensorAHTxx::get_properties(void) {
    return (SensorAHTxx::athxx_properties);
}

size_t SensorAHTxx::get_property_count(void) {
    return (SIZEOFARRAY(SensorAHTxx::athxx_properties));
}

void SensorAHTxx::init(void) {
	device_address = MULTI_ADDR_AHT10_AHT21;
    reading.setup(get_properties(), get_property_count());
    xTaskCreate(SensorAHTxx::_driverTask, "SensorAHTxx", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY - 1, &task_handle);
}

void SensorAHTxx::cleanup(void) {
	disable();
    vTaskDelete(task_handle);
}

esp_err_t SensorAHTxx::enable(void) {
    if (swI2CPort.detect(device_address, I2C_MAX_RETRY) != ESP_OK) {
        return (ESP_FAIL);
    }

	esp_err_t result = soft_reset();
	if (result != ESP_OK) {
		result = init_registers();
        vTaskDelay(pdMS_TO_TICKS(AHTXX_CMD_DELAY));
	}
	
	result = calibration();
	if (result == ESP_OK) {
		result = read_serial_number_code(head_serial_number);
		if (result == ESP_OK) {
			set_normal_mode();
			set_loop_time(AHTXX_LOOP_TIME_MS);
			initialized = true;
		}
	}

	return (result);
}

esp_err_t SensorAHTxx::disable(void) {
	swI2CPort.disable();
	initialized = false;
    return (ESP_OK);
}

esp_err_t SensorAHTxx::read_serial_number_code(uint32_t& serialNumberCode) {
	serialNumberCode = SENSOR_SERIAL_NUMBER_NOT_SUPPORTED;
	return (ESP_OK);
}

uint8_t SensorAHTxx::calcCRC8(uint8_t* message, size_t length) {
	uint8_t crc = 0xff;
	for (size_t byte = 0; byte < length; byte++) {
		crc ^= message[byte];
		for (int i = 0; i < 8; i++) {
			if (crc & 0x80) {
				crc = (crc << 1) ^ 0x31;
			} else {
				crc = (crc << 1);
			}
		}
	}
	return (crc);
}

esp_err_t SensorAHTxx::read_status_register(uint8_t& status) {
    return (swI2CPort.read_register(device_address, AHTXX_STATUS_REG, status));
}

bool SensorAHTxx::check_status_bit(uint8_t mask) {
	uint8_t status = 0xff;
	if ((read_status_register(status) == ESP_OK) && (status != 0xff)) {
		return ((status & mask) == mask);
	}
	return (false);
}

bool SensorAHTxx::is_ready(void) {
	return (check_status_bit(AHTXX_STATUS_OK));
}

bool SensorAHTxx::is_busy(void) {
	return (check_status_bit(AHTXX_STATUS_CTRL_BUSY));
}

esp_err_t SensorAHTxx::soft_reset(void) {
	esp_err_t result = swI2CPort.send_command(device_address, AHTXX_SOFT_RESET_REG);
	if (result != ESP_OK) {
		return (result);
	}

	result = wait_for_operation(AHTXX_SOFT_RESET_DELAY);
	if (result != ESP_OK) {
		return (result);
	}

	return ((is_ready()) ? ESP_OK : ESP_FAIL);
}

esp_err_t SensorAHTxx::wait_for_operation(uint32_t timeout_ms) {
	int loops = pdMS_TO_TICKS(timeout_ms);
	do {
		vTaskDelay(pdMS_TO_TICKS(1));
		if (!is_busy()) {
			return (ESP_OK);
		}
	} while (--loops > 0);
	return (ESP_ERR_TIMEOUT);
}

esp_err_t SensorAHTxx::set_initialization_register(uint8_t value) {
	uint8_t cmd1[3] = { AHT1X_INIT_REG, value, AHTXX_START_MEASUREMENT_CTRL_NOP };
    swI2CPort.send_command_sequence(device_address, cmd1, sizeof (cmd1));
	
	uint8_t cmd2[3] = { AHT2X_INIT_REG, value, AHTXX_START_MEASUREMENT_CTRL_NOP };
    swI2CPort.send_command_sequence(device_address, cmd2, sizeof (cmd2));
	
	return (ESP_OK);
}

esp_err_t SensorAHTxx::set_normal_mode(void) {
	return (set_initialization_register(AHTXX_INIT_CTRL_CAL_ON | AHT1X_INIT_CTRL_NORMAL_MODE));
}

esp_err_t SensorAHTxx::set_cycle_mode(void) {
	return (set_initialization_register(AHTXX_INIT_CTRL_CAL_ON | AHT1X_INIT_CTRL_CYCLE_MODE));
}

esp_err_t SensorAHTxx::set_command_mode(void) {
	return (set_initialization_register(AHTXX_INIT_CTRL_CAL_ON | AHT1X_INIT_CTRL_CMD_MODE));
}

esp_err_t SensorAHTxx::calibration(void) {
	uint8_t cmd[3]{ AHTXX_CMD_CALIBRATE, 0x08, 0x00 };
	swI2CPort.send_command_sequence(device_address, cmd, sizeof (cmd));
	esp_err_t result = wait_for_operation(ATHXX_CALIBRATION_TIMEOUT_MS);
	if (result == ESP_OK) {
		uint8_t status = 0x00;
		result = read_status_register(status);
		if (result == ESP_OK) {
			if ((status & AHTXX_STATUS_CTRL_CAL_ON) != AHTXX_STATUS_CTRL_CAL_ON) {
				result = ESP_FAIL;
			}
		}
	}
	return (result);
}

esp_err_t SensorAHTxx::reset_register(uint8_t regAddr) {
	const uint8_t cmd[3] = { regAddr, AHTXX_INIT_CTRL_NOP, AHTXX_INIT_CTRL_NOP };
	uint8_t i2c_buffer[3]{0xff};
	esp_err_t result = swI2CPort.transact(device_address, cmd, sizeof (cmd), i2c_buffer, sizeof (i2c_buffer), AHTXX_REG_RESET_DELAY);
	if (result != ESP_OK) {
		return (result);
	}
	i2c_buffer[0] = 0xB0 | regAddr;
	return (swI2CPort.send_command_sequence(device_address, i2c_buffer, sizeof (i2c_buffer)));
}

esp_err_t SensorAHTxx::init_registers(void) {
	esp_err_t result = reset_register(0x1b);
	if (result == ESP_OK) {
		result = reset_register(0x1c);
	}
	if (result == ESP_OK) {
		result = reset_register(0x1e);
	}
	return (result);
}

const char* SensorAHTxx::get_head(void) {
    return (headName);
}

esp_err_t SensorAHTxx::set_heating(bool enable) {
	return (ESP_FAIL);
}

uint8_t SensorAHTxx::get_sub_type(void) {
	return (typeID);
}

esp_err_t SensorAHTxx::update(void) {
    reading.set_Value(RK_T,  temperature_C);
    reading.set_Value(RK_RH, humidity_RH);
    reading.set_Value(RK_DP, dewpoint_C);
    reading.set_Value(RK_AH, humidity_AH);
	return (ESP_OK);
}

esp_err_t SensorAHTxx::fetch(void) {
	if (is_busy()) {
		return (ESP_OK);
	}

	const uint8_t commandSequence[3] = { AHTXX_START_MEASUREMENT_REG, AHTXX_START_MEASUREMENT_CTRL, AHTXX_START_MEASUREMENT_CTRL_NOP };
	esp_err_t result = swI2CPort.send_command_sequence(device_address, commandSequence, sizeof (commandSequence));
	if (result == ESP_OK) {
		wait_for_operation(ATHXX_MEASURING_TIMEOUT_MS);

		uint8_t i2c_buffer[7]{ 0xff };
		result = swI2CPort.burst_read(device_address, i2c_buffer, sizeof (i2c_buffer));
		swI2CPort.stop();

		if (result == ESP_OK) {
			uint8_t crc = 0xff;
			if (i2c_buffer[6] != 0xff) {
				headName = AHT10_DEVICE_NAME;
				typeID   = ATHXX_TYPE_ATH10;
				crc = calcCRC8(i2c_buffer, 6);
				if (crc != i2c_buffer[6]) {
					result = ESP_FAIL;
				}
			} else {
				headName = AHT21_DEVICE_NAME;
				typeID   = ATHXX_TYPE_ATH21;
			}

			if (result == ESP_OK) {
				rawDataRH = ((uint32_t)i2c_buffer[1] << 12) | ((uint32_t)i2c_buffer[2] << 4) | ((uint32_t)i2c_buffer[3] >> 4);
				humidity_RH = MAX(0.0f, MIN(100.0f, ((float)rawDataRH / (float)0x0fffff) * 100.0f));
				rawDataTemp = (((uint32_t)i2c_buffer[3] & 0x0f) << 16) | ((uint32_t)i2c_buffer[4] << 8) | (uint32_t)i2c_buffer[5];
				temperature_C = MAX(-40.0f, MIN(+85.0f, ((float)rawDataTemp / (float)0x0fffff) * 200.0f - 50.0f));
				apply_magnus_formula(temperature_C, humidity_RH, dewpoint_C, humidity_AH);

#ifdef DISPLAY_STATE
            ESP_LOGI(TAG,
                "%s reading: T=%.1f°C (0x%-4.4X) RH=%.1f%% (0x%-4.4X) DP=%.1f, AH=%.1f",
				get_head(), temperature_C, (unsigned int)rawDataTemp, humidity_RH, (unsigned int)rawDataRH, dewpoint_C, humidity_AH);
#endif

				return (ESP_OK);
			}
		}
	}

	return (ESP_FAIL);
}

SensorType SensorAHTxx::get_sensor_type() {
    return (SensorType::ATHxx);
}
