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

//#define DISPLAY_STATE

esp_err_t DisplayLCD16x2::send_command(uint8_t cmd) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C;
	data_t[1] = data_u | 0x08;
	data_t[2] = data_l | 0x0C;
	data_t[3] = data_l | 0x08;
    esp_err_t result = i2c_master_write_to_device(port, slave_address, data_t, SIZEOFARRAY(data_t), LCD_IO_TIMEOUT_MS);
#ifdef DISPLAY_STATE
    if (result != ESP_OK) {
        ESP_LOGI(TAG, "Command error %d. ", (int)result);
    }
#endif
    return (result);
}

esp_err_t DisplayLCD16x2::send_data(char data) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D;
	data_t[1] = data_u | 0x09;
	data_t[2] = data_l | 0x0D;
	data_t[3] = data_l | 0x09;
    esp_err_t result = i2c_master_write_to_device(port, slave_address, data_t, SIZEOFARRAY(data_t), LCD_IO_TIMEOUT_MS);
#ifdef DISPLAY_STATE
    if (result != ESP_OK) {
        ESP_LOGI(TAG, "Data error %d. ", (int)result);
    }
#endif
    return (result);
}

esp_err_t DisplayLCD16x2::init(void) {
    memset(&displayBuffer, 0, sizeof (displayBuffer));

    vTaskDelay(pdMS_TO_TICKS(100));
	if (send_command(0x30) != ESP_OK) return (ESP_FAIL);

    vTaskDelay((6));
	if (send_command(0x30) != ESP_OK) return (ESP_FAIL); 
    vTaskDelay((2));
	if (send_command(0x30) != ESP_OK) return (ESP_FAIL);
    vTaskDelay((2));

	if (send_command(0x20) != ESP_OK) return (ESP_FAIL); // 4-bit mode
    vTaskDelay((2));
	if (send_command(0x28) != ESP_OK) return (ESP_FAIL); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
    vTaskDelay((2));
	if (send_command(0x08) != ESP_OK) return (ESP_FAIL); // Display on/off control --> D=0,C=0, B=0  ---> display off
    vTaskDelay((2));
	if (send_command(0x01) != ESP_OK) return (ESP_FAIL); // clear display
    vTaskDelay((2));
	if (send_command(0x06) != ESP_OK) return (ESP_FAIL); // Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
    vTaskDelay((2));
	if (send_command(0x0C) != ESP_OK) return (ESP_FAIL); // Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
    vTaskDelay((2));

    f_available = true;

    return (ESP_OK);
}

esp_err_t DisplayLCD16x2::cursor_pos(int row, int col) {
    if (!f_available) {
        return (ESP_ERR_NOT_SUPPORTED);
    }

    if ((row < 0) || (row > 1) || (col < 0) || (col > 15)) {
#ifdef DISPLAY_STATE
        ESP_LOGE(TAG, "Position error (%d,%d). ", row, col);
#endif
        return (ESP_FAIL);
    }

    return (send_command((row == 0) ? (col | 0x80) : (col | 0xC0)));
}

esp_err_t DisplayLCD16x2::update(void) {
    if (!f_available) {
        return (ESP_ERR_NOT_SUPPORTED);
    }

#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "DisplayLCD16x2::update(\"%-32.32s\"", displayBuffer.s);
#endif
    esp_err_t result = ESP_OK;
    for (int row = 0; row < 2; row++) {
        result = cursor_pos(row, 0);
        if (result != ESP_OK) break;
        for (int col = 0; col < 16; col++) {
            result = send_data(displayBuffer.a[row][col]);
            if (result != ESP_OK) {
                return (result);
            }
        }
    }
    return (ESP_OK);
}

esp_err_t DisplayLCD16x2::clear(void) {
    memset(displayBuffer.s, ' ', sizeof (displayBuffer.s));
    return (ESP_OK);
}

esp_err_t DisplayLCD16x2::setLogo(void) {
    print(0, 0, " KKS-Elektronik ");
    print(0, 0, "    (C)2026     ");
    return (ESP_OK);
}

esp_err_t DisplayLCD16x2::print_large(int x, int y, const char* text, size_t length) {
    return (print(x, y, text, length));
}

esp_err_t DisplayLCD16x2::print(int x, int y, const char* text, size_t length) {
    if (length == 0) {
        length = (size_t)((int)strlen(text) & 0x0f);
    }
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "DisplayLCD16x2::print(%d, %d, \"%s\", %d)", x, y, text, length);
#endif
    if (text == nullptr) {
        clear();
        return (ESP_OK);
    }

    int i = (((y % 2) * 16) + (x % 16)) & 0x1f;
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "DisplayLCD16x2::print(%d, %d, \"%s\", %d) -> i=%d", x, y, text, length, i);
#endif
    for (int n = 0; (n < length) && (text[n] != 0) && (i < 32); n++) {
        displayBuffer.s[i++] = text[n];
    }

    return (ESP_OK);
}

esp_err_t DisplayLCD16x2::invert_row(int row) {
    return (ESP_FAIL);
}

esp_err_t DisplayLCD16x2::set_rotation(uint8_t mask) {
    return (ESP_FAIL);
}

esp_err_t DisplayLCD16x2::set_contrast(float value) {
    return (ESP_OK);
}

esp_err_t DisplayLCD16x2::off(void) {
    return (ESP_OK);
}

esp_err_t DisplayLCD16x2::on(void) {
    return (ESP_OK);
}
