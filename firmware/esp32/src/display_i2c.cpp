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

#include "app.hpp"

// #define DISPLAY_STATE

void DisplayI2C::init(i2c_port_t lcdPort, gpio_num_t pinSDA, gpio_num_t pinSCL) {
    port = lcdPort;
    sda = pinSDA;
    scl = pinSCL;
    if ((pinSDA != GPIO_NUM_NC) && (pinSCL != GPIO_NUM_NC)) {
        portSetup();
        detect_display_type();
    }
    mutex = xSemaphoreCreateMutex();
    esp_event_post(APP_EVENT, (int32_t)AppEvent::display_ready, nullptr, 0, pdMS_TO_TICKS(10));
}

void DisplayI2C::cleanup(void) {
    i2c_driver_delete(port);
    if (mutex != nullptr) {
        vSemaphoreDelete(mutex);
        mutex = nullptr;
    }
    esp_event_post(APP_EVENT, (int32_t)AppEvent::display_off, nullptr, 0, pdMS_TO_TICKS(1));
}

void DisplayI2C::portSetup(void) {
    i2c_config_t iic_setup;
    memset(&iic_setup, 0, sizeof (iic_setup));
    iic_setup.mode = I2C_MODE_MASTER;
    iic_setup.sda_io_num = sda;
    iic_setup.scl_io_num = scl;
    iic_setup.sda_pullup_en = GPIO_PULLUP_ENABLE;
    iic_setup.scl_pullup_en = GPIO_PULLUP_ENABLE;
    iic_setup.master.clk_speed = LCD_IIC_FREQ_HZ;
    iic_setup.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;
    i2c_param_config(port, &iic_setup);
    i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, 0);
}

esp_err_t DisplayI2C::detect_display_type(void) {
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_err_t result = ESP_FAIL;
    uint8_t testData[2]{ 0 };
    for (int i = 0x00; i < 128; i++) {
        if (i2c_master_write_to_device(port, (uint8_t)(i & 0x7f), testData, SIZEOFARRAY(testData), LCD_IO_TIMEOUT_MS) == ESP_OK) {
            if ((i == LCD16X2_A0) ||
                (i == LCD16X2_A1) || 
                (i == LCD16X2_A2) || 
                (i == LCD16X2_A3) || 
                (i == LCD16X2_A4) || 
                (i == LCD16X2_A5) || 
                (i == LCD16X2_A6) || 
                (i == LCD16X2_A7) ||
                (i == LCD16X2_A8)) {
                create_display(DisplayType::LCD16x2, (uint8_t)(i & 0x7f), 0);
            } else if (i == OLED_128X64_A0) {
                create_display(DisplayType::OLED128x64, (uint8_t)(i & 0x7f), 0);
            }
        }
    }
    return (result);
}

DisplayHandle DisplayI2C::create_display(DisplayType type, uint8_t I2Caddr, uint8_t rotationMask) {
    DisplayHandle instance = nullptr;
    switch (type) {
        case DisplayType::LCD16x2: {
            instance = new DisplayLCD16x2(port, type, I2Caddr, rotationMask);
        } break;

        case DisplayType::OLED128x64: {
            instance = new DisplayOLED128x64(port, type, I2Caddr, rotationMask);
        } break;

        default: {
        } break;
    }
    if (instance != nullptr) {
        instance->next = control;
        control = instance;
        instance->init();
        instance->clear();
        instance->update();
        displayCount++;
    }
    return ((DisplayHandle)instance);
}

esp_err_t DisplayI2C::clear(void) {
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        handle->clear();
        handle = handle->next;
    }
    return (ESP_OK);
}

esp_err_t DisplayI2C::setLogo(void) {
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        handle->setLogo();
        handle = handle->next;
    }
    return (ESP_OK);
}

esp_err_t DisplayI2C::update(void) {
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        if (mutex != nullptr) {
            if(xSemaphoreTake(mutex, ( TickType_t )10 ) == pdTRUE) {
                handle->update();
                xSemaphoreGive(mutex);
            }
        }
        handle = handle->next;
    }
    return (ESP_OK);
}

esp_err_t DisplayI2C::off(void) {
    ESP_LOGI(TAG, "DisplayI2C::off()");
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        handle->off();
    }
    return (ESP_OK);
}

esp_err_t DisplayI2C::on(void) {
    ESP_LOGI(TAG, "DisplayI2C::on()");
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        handle->on();
    }
    return (ESP_OK);
}

esp_err_t DisplayI2C::set_rotation(uint8_t mask) {
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        handle->set_rotation(mask);
        handle->update();
    }
    return (ESP_OK);
}

esp_err_t DisplayI2C::print(int x, int y, const char* text, size_t length) {
    if (text == nullptr) {
        return (ESP_OK);
    }

    if (length == 0) {
        length = strlen(text);
    }

    if (length > 0) {
        DisplayHandle handle = control;
        for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
            handle->print(x, y, text, length);
            handle = handle->next;
        }
    }

    return (ESP_OK);
}

esp_err_t DisplayI2C::print_large(int x, int y, const char* text, size_t length) {
    if (text == nullptr) {
        return (ESP_OK);
    }

    if (length == 0) {
        length = strlen(text);
    }

    if (length > 0) {
        DisplayHandle handle = control;
        for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
            handle->print_large(x, y, text, length);
            handle = handle->next;
        }
    }
    return (ESP_OK);
}

esp_err_t DisplayI2C::invert_row(int row) {
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        handle->invert_row(row);
        handle = handle->next;
    }
    return (ESP_OK);
}

esp_err_t DisplayI2C::set_contrast(float value) {
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        handle->set_contrast(value);
        handle = handle->next;
    }
    return (ESP_OK);
}

bool DisplayI2C::has_DisplayOfType(DisplayType type) {
    DisplayHandle handle = control;
    for (int i = 0; (i < displayCount) && (handle != nullptr); i++) {
        if (handle->type == type) {
            return (true);
        }
        handle = handle->next;
    }
    return (false);
}
