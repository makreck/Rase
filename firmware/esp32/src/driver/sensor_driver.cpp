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

static const uint8_t i2c_driver_adr_list[] = {
    MULTI_ADDR_AHT10_AHT21,         // 0x38
    MULTI_ADDR_HDC1080_HTU21_SHT2X, // 0x40
    SHT3_DEVICE_ADDRESS,            // 0x44
    MULTI_ADDR_BMP280_BME280,       // 0x76
};

static const char* i2c_driver_name_list[] = {
    ADR_38_DEV_NAMES,
    ADR_40_DEV_NAMES,
    ADR_44_DEV_NAMES,
    ADR_76_DEV_NAMES,
};

static const SensorType i2c_driver_type_list[] = {
    SensorType::ATHxx,
    SensorType::autoscan,
    SensorType::SHT3x,
    SensorType::BMx280,
};

void SensorDriver::send_measuring_event(void) {
    esp_event_post(APP_EVENT, (int32_t)AppEvent::measuring_event, &reading, sizeof(reading), pdMS_TO_TICKS(0));
}

void SensorDriver::apply_magnus_formula(float temperature_C, float humidity_RH, float &dewpoint_C, float &humidity_AH) {
    float saturationVaporPressure_hPa = 6.1078f * exp((17.08085f * temperature_C) / (234.175f + temperature_C));
    if (temperature_C < 0.0) {
        saturationVaporPressure_hPa *= exp(0.00972f * temperature_C);
    }

    float vaporPressure_hPa = humidity_RH * saturationVaporPressure_hPa / 100.0f;
    if (vaporPressure_hPa != 0.0f) {
        float f = log(fabsf(vaporPressure_hPa / 6.1078f));
        dewpoint_C = 234.175f * f / (17.08085f - f);
    } else {
        dewpoint_C = (float)(-273.15f);
    }

    humidity_AH = (vaporPressure_hPa / (461.52f * (temperature_C + 273.15f))) * 100000.0f;
}

bool SensorDriver::wait_ready(uint32_t timeout_ms) {
    while (!initialized) {
        vTaskDelay(pdMS_TO_TICKS(1));
        if (--timeout_ms < 1) {
            return (false);
        }
    }
    return (true);
}

void SensorDriver::set_loop_time(TickType_t time_ms) {
    loop_time_ms = (TickType_t)std::max(10, std::min(5000, (int)time_ms));
}

void SensorDriver::_driverTask(void* pvParameters) {
    (reinterpret_cast<SensorDriver*>(pvParameters))->driverTask();
}
void SensorDriver::driverTask(void) {
    error = true;
    while(true) {
        if (suspended) {
            running = false;
            vTaskDelay(pdMS_TO_TICKS(SENSOR_SUSPEND_DELAY_MS));
            continue;
        }

        running = true;

        if (error) {
            vTaskDelay(pdMS_TO_TICKS(SENSOR_STD_RETRY_DELAY_MS));
            if (enable() != ESP_OK) {
                continue;
            }
            reading.setup(get_properties(), get_property_count());
            esp_event_post(APP_EVENT, (int32_t)AppEvent::driver_ready, &reading, sizeof (reading), pdMS_TO_TICKS(10));
        }
 
        vTaskDelay(pdMS_TO_TICKS(loop_time_ms));
        if (fetch() == ESP_OK) {
            error = false;
            update();
            send_measuring_event();
        }
    }
}

uint8_t SensorDriver::get_bus_addr_by_type(SensorType type) {
    switch (type) {
        case SensorType::SHT3x: {
        } return (SHT3_DEVICE_ADDRESS);

        case SensorType::SHT2x: {
        } return (MULTI_ADDR_HDC1080_HTU21_SHT2X);

        case SensorType::HTU21d: {
        } return (MULTI_ADDR_HDC1080_HTU21_SHT2X);

        case SensorType::HDC1080: {
        } return (MULTI_ADDR_HDC1080_HTU21_SHT2X);

        case SensorType::ATHxx: {
        } return (MULTI_ADDR_AHT10_AHT21);

        case SensorType::BMx280: {
        } return (MULTI_ADDR_BMP280_BME280);
        
        case SensorType::autoscan:
        case SensorType::invalid:
        default: {
        } break;
    }
    return (INVALID_DEVICE_ADDRESS);
}

SensorDriver* SensorDriver::create_driver_by_address(uint8_t bus_addr) {
    if (bus_addr == SHT3_DEVICE_ADDRESS) {
        return (new SensorSHT3());
    } else if (bus_addr == MULTI_ADDR_BMP280_BME280) {
        return (new SensorBM2x());
    } else if (bus_addr == MULTI_ADDR_AHT10_AHT21) {
        return (new SensorAHTxx());
    } else if (bus_addr == MULTI_ADDR_HDC1080_HTU21_SHT2X) {
        SwI2CBus i2c_bus;
        if (SensorSHT2::is_connected(i2c_bus)) {
            return (new SensorSHT2());
        }
        if (SensorHTU21d::is_connected(i2c_bus)) {
            return (new SensorHTU21d());
        }
        if (SensorHDC1080::is_connected(i2c_bus)) {
            return (new SensorHDC1080());
        }
    }
    return (new SensorNull());
}

SensorDriver* SensorDriver::auto_scan(SensorType selected) {
    uint8_t busAddressList[] = {
        get_bus_addr_by_type(selected),
        SHT3_DEVICE_ADDRESS,            // 0x44
        MULTI_ADDR_HDC1080_HTU21_SHT2X, // 0x40
        MULTI_ADDR_BMP280_BME280,       // 0x76
        MULTI_ADDR_AHT10_AHT21,         // 0x38
    };

    for (size_t i = 0; i < SIZEOFARRAY(busAddressList); i++) {
        SwI2CBus i2c_bus;
        uint8_t bus_addr = busAddressList[i];
        if (bus_addr == INVALID_DEVICE_ADDRESS) continue;
        if (i2c_bus.detect(busAddressList[i], I2C_MAX_RETRY) == ESP_OK) {
            return (create_driver_by_address(bus_addr));
        }
    }
    return (new SensorNull());
}

uint32_t SensorDriver::get_head_serial_number(void) {
    return (head_serial_number);
}

SensorReading* SensorDriver::get_reading(void) {
    return (&reading);
}

bool SensorDriver::suspend(void) {
    suspended = true;
    for (int n = 0; (n < 10) && running; n++) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return (!running);
}

size_t SensorDriver::search(size_t length, const char* result[], uint8_t* adr_list, SensorType* typeList) {
    if (!suspend()) {
        return (0);
    }
    size_t n = 0;
    for (size_t i = 0; i < sizeof (i2c_driver_adr_list); i++) {
        if (swI2CPort.detect(i2c_driver_adr_list[i], I2C_MAX_RETRY) == ESP_OK) {
            if (n < length) {

                // Possibly HDC1080 or SHT2x or HTU21d or both connected all to I2C address 0x40:
                if (i2c_driver_adr_list[i] == MULTI_ADDR_HDC1080_HTU21_SHT2X) {

                    if (SensorSHT2::is_connected(swI2CPort) && (n < length)) {
                        if (adr_list != nullptr) adr_list[n] = SHT2_DEVICE_ADDRESS;
                        if (typeList != nullptr) typeList[n] = SensorType::SHT2x;
                        if (result   != nullptr) result[n] = SHT2_DEVICE_NAME;
                        n++;
                        ESP_LOGI(TAG, "Detected (%d) at 0x%-2.2X = \"%s\" ", (int)(n + 1), SHT2_DEVICE_ADDRESS, SHT2_DEVICE_NAME);
                    }

                    if (SensorHTU21d::is_connected(swI2CPort) && (n < length)) {
                        if (adr_list != nullptr) adr_list[n] = HTU21_DEVICE_ADDRESS;
                        if (typeList != nullptr) typeList[n] = SensorType::HTU21d;
                        if (result   != nullptr) result[n] = HTU21_DEVICE_NAME;
                        n++;
                        ESP_LOGI(TAG, "Detected (%d) at 0x%-2.2X = \"%s\" ", (int)(n + 1), HTU21_DEVICE_ADDRESS, HTU21_DEVICE_NAME);
                    }

                    if (SensorHDC1080::is_connected(swI2CPort) && (n < length)) {
                        if (adr_list != nullptr) adr_list[n] = HDC1080_DEVICE_ADDRESS;
                        if (typeList != nullptr) typeList[n] = SensorType::HDC1080;
                        if (result   != nullptr) result[n] = HDC1080_DEVICE_NAME;
                        n++;
                        ESP_LOGI(TAG, "Detected (%d) at 0x%-2.2X = \"%s\" ", (int)(n + 1), HDC1080_DEVICE_ADDRESS, HDC1080_DEVICE_NAME);
                    }

                } else {
                    ESP_LOGI(TAG, "Detected (%d) at 0x%-2.2X = \"%s\" ", (int)(n + 1), i2c_driver_adr_list[i], i2c_driver_name_list[i]);
                    if (adr_list != nullptr) adr_list[n] = i2c_driver_adr_list[i];
                    if (typeList != nullptr) typeList[n] = i2c_driver_type_list[i];
                    if (result != nullptr) result[n] = i2c_driver_name_list[i];
                    n++;
                }
            }
        }
    }
    suspended = false;
    return (n);
}
