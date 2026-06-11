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

const SensorProperty SensorNull::null_properties[] = {
    { "sim_1", "Simulation #1", "X1", "%", 0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#ff0000", 1001 },
    { "sim_2", "Simulation #2", "X2", "%", 0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#24d800", 1002 },
    { "sim_3", "Simulation #3", "X3", "%", 0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#0011ff", 1003 },
    { "sim_4", "Simulation #4", "X4", "%", 0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#d9ff00", 1004 },
    { "sim_5", "Simulation #5", "X5", "%", 0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#ff00bf", 1005 },
    { "sim_6", "Simulation #6", "X6", "%", 0.0f, 100.0f, SP_FLAGS(1, 2, false, false), "#7700ff", 1006 },
};

const SensorProperty* SensorNull::get_properties(void) {
    return (SensorNull::null_properties);
}

size_t SensorNull::get_property_count(void) {
    return (SIZEOFARRAY(SensorNull::null_properties));
}

esp_err_t SensorNull::init(void) {
    device_address = INVALID_DEVICE_ADDRESS;
    reading.setup(get_properties(), get_property_count());
    xTaskCreate(SensorNull::_driverTask, "SensorNull", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY - 1, &task_handle);
    return (ESP_OK);
}

esp_err_t SensorNull::cleanup(void) {
    vTaskDelete(task_handle);
    task_handle = nullptr;
    return (ESP_OK);
}

const char* SensorNull::get_head(void) {
    return (headName);
}

esp_err_t SensorNull::enable(void) {
    // if (swI2CPort.detect(device_address, I2C_MAX_RETRY) != ESP_OK) {
    //     return (ESP_FAIL);
    // }
    for (int i = 0; i < SIZEOFARRAY(sim_value); i++) {
        sim_value[i] = Tools::get_random() * 80.0f + 10.0f;
    }
    head_serial_number = (uint32_t)0x12345678;
    initialized = true;
    return (ESP_OK);
}

esp_err_t SensorNull::disable(void) {
    swI2CPort.disable();
    initialized = false;
    return (ESP_OK);
}

esp_err_t SensorNull::set_heating(bool enable) {
    return (ESP_FAIL);
}

uint8_t SensorNull::get_sub_type(void) {
    return (0);
}

SensorType SensorNull::get_sensor_type() {
    return (SensorType::Null);
}

esp_err_t SensorNull::update(void) {
    reading.set_Value("sim_1", sim_value[0]);
    reading.set_Value("sim_2", sim_value[1]);
    reading.set_Value("sim_3", sim_value[2]);
    reading.set_Value("sim_4", sim_value[3]);
    reading.set_Value("sim_5", sim_value[4]);
    reading.set_Value("sim_6", sim_value[5]);
    return (ESP_OK);
}

esp_err_t SensorNull::fetch(void) {
    if (!initialized) {
        return (ESP_FAIL);
    }

    for (int i = 0; i < SIZEOFARRAY(sim_value); i++) {
        if (sim_value[i] > 75.0f) {
            sim_value[i] -= fabsf(Tools::get_random());
        } else if (sim_value[i] < 25.0f) {
            sim_value[i] += fabsf(Tools::get_random());
        } else {
            sim_value[i] += (Tools::get_random() * 2.0f - 1.0f);
        }
    }

    return (ESP_OK);
}
