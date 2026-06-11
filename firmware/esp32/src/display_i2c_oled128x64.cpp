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

#define SSD1306_CMD_SET_COLUMN_LOW          (0x00)
#define SSD1306_CMD_SET_COLUMN_HIGH         (0x10)
#define SSD1306_CMD_SET_MEMORY_ADDR_MODE    (0x20)
#define SSD1306_CMD_SET_COLUMN_RANGE        (0x21)
#define SSD1306_CMD_SET_PAGE_RANGE          (0x22)
#define SSD1306_CMD_SET_START_LINE          (0x40)
#define SSD1306_CMD_SET_DISPLAY_CLOCK_EN    (0x80)
#define SSD1306_CMD_SET_CONTRAST            (0x81)
#define SSD1306_CMD_SET_DC_DC_ENABLE        (0x8d)
#define SSD1306_CMD_SET_SEGMENT_REMAP       (0xa1) // 0 ... 127
#define SSD1306_CMD_OUTPUT_FOLLOWS_RAM      (0xa4)
#define SSD1306_CMD_OUTPUT_IGNORES_RAM      (0xa5)
#define SSD1306_CMD_NORMAL_DISPLAY          (0xa6)
#define SSD1306_CMD_INVERT_DISPLAY          (0xa7)
#define SSD1306_CMD_SET_MULTIPLEX           (0xa8)
#define SSD1306_CMD_DISPLAY_OFF             (0xae)
#define SSD1306_CMD_DISPLAY_ON              (0xaf)
#define SSD1306_CMD_SET_PAGE_START_ADDRESS  (0xb0) // 0xb0 ... 0xb7
#define SSD1306_CMD_SET_COM_SCAN_DEC        (0xc0)
#define SSD1306_CMD_SET_COM_SCAN_INC        (0xc8)
#define SSD1306_CMD_SET_DISPLAY_OFFSET      (0xd3)
#define SSD1306_CMD_SET_DISPLAY_CLOCK       (0xd5)
#define SSD1306_CMD_SET_PRECHARGE_PERIOD    (0xd9)
#define SSD1306_CMD_SET_COM_PIN_CONFIG      (0xda)
#define SSD1306_CMD_SET_VCOM_DESELECT       (0xdb)
#define SSD1306_CMD_SET_DIVIDE_RATIO        (0xf0)

#define SSD1306_PAR_SEQUENTIAL_COM_PIN      (0x12)
#define SSD1306_PAR_CONTRAST_VALUE          (0xFF) // 0xCF vs. 0xDF vs. 0xFF
#define SSD1306_PAR_MULTIPLEX_DUTY_CYCLE    (0x3F) // 1/64 duty cycle
#define SSD1306_PAR_PRECHARGED_PERIOD_TIME  (0x22) // (0xF1)
#define SSD1306_PAR_VCOMH_DESELECT_LEVEL    (0x20) // (0x40)
#define SSD1306_PAR_DISPLAY_OFFSET          (0x00) // no offset
#define SSD1306_PAR_DC_DC_ENABLE            (0x14) // DC-DC enable
#define SSD1306_PAR_DC_DC_DISABLE           (0x10) // DC-DC disable
#define SSD1306_PAR_CLOCK_DIVIDE_RATIO      (0xF0) // Display clock divide ratio
#define SSD1306_PAR_ADRMODE_HORZ            (0x00) // Page addressing mode: horizontal
#define SSD1306_PAR_ADRMODE_VERT            (0x01) // Page addressing mode: vertical
#define SSD1306_PAR_ADRMODE_PAGE            (0x10) // Page addressing mode: pages
#define SSD1306_PAR_ADRMODE_INV             (0x11) // Page addressing mode: invalid


static const char ssd1306_init_sequence[] = {
    SSD1306_CMD_DISPLAY_OFF,               // display off

    SSD1306_CMD_SET_MEMORY_ADDR_MODE,      // set memory addressing mode	
	SSD1306_PAR_ADRMODE_PAGE,              // 0x00 = horizontal addressing mode, 0x01 = vertical addressing mode, 0x10 = page addressing mode (reset), 0x11 = invalid

	SSD1306_CMD_SET_PAGE_START_ADDRESS,    // set page start address for page addressing mode, +0 to +7
	
    SSD1306_CMD_SET_COM_SCAN_INC,          // set COM output scan direction
	
    SSD1306_CMD_SET_COLUMN_LOW,            // set low column address
	SSD1306_CMD_SET_COLUMN_HIGH,           // set high column address
	SSD1306_CMD_SET_START_LINE,            // set start line address
	
    SSD1306_CMD_SET_CONTRAST,              // set contrast control register
	SSD1306_PAR_CONTRAST_VALUE,            // contrast value 0x00 ... 0xFF	

    SSD1306_CMD_SET_SEGMENT_REMAP,         // set segment re-map 0 to 127

    SSD1306_CMD_NORMAL_DISPLAY,            // set normal display

    SSD1306_CMD_SET_MULTIPLEX,             // set multiplex ratio
	SSD1306_PAR_MULTIPLEX_DUTY_CYCLE,      // multiplex ratio (duty cycle) 1 to 64

    SSD1306_CMD_OUTPUT_FOLLOWS_RAM,        // 0xA4 = output follows RAM content, 0xa5 = output ignores RAM content

    SSD1306_CMD_SET_DISPLAY_OFFSET,        // set display offset
	SSD1306_PAR_DISPLAY_OFFSET,            // no offset

    SSD1306_CMD_SET_DISPLAY_CLOCK,         // set display clock divide ratio/oscillator frequency
	SSD1306_PAR_CLOCK_DIVIDE_RATIO,        // clock divide ratio

	SSD1306_CMD_SET_PRECHARGE_PERIOD,      // set pre-charge period
	SSD1306_PAR_PRECHARGED_PERIOD_TIME,    // period time

	SSD1306_CMD_SET_COM_PIN_CONFIG,        // set com pins hardware configuration
	SSD1306_PAR_SEQUENTIAL_COM_PIN,        //
	
    SSD1306_CMD_SET_VCOM_DESELECT,         // set Vcom H
	SSD1306_PAR_VCOMH_DESELECT_LEVEL,      // 0x20 = 0.77 x Vcc

	SSD1306_CMD_SET_DC_DC_ENABLE,          // set DC-DC enable
	SSD1306_PAR_DC_DC_ENABLE,              // DC-DC enable part #2 (enable = 0x14, disable = 0x10)

	SSD1306_CMD_DISPLAY_ON,                // turn on OLED panel 
};

esp_err_t DisplayOLED128x64::init(void) {
    for (int i = 0; i < sizeof (ssd1306_init_sequence); i++) {
        send_command(ssd1306_init_sequence[i]);
    }
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::clear(void) {
    memset(displayBuffer.s, 0, sizeof (displayBuffer.s));
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::setLogo(void) {
    if (swap_vertical) {
        int n = 1023;
        for (int i = 0; i < 1024; i++, n--) {
            displayBuffer.s[(i + 256) & 0x3ff] = bitReverseTab[kks_logo_image[n & 0x3ff]];
        }
        print(0, 3, "   Powered by   ", 16);
    } else {
        memcpy(displayBuffer.s, kks_logo_image, sizeof (displayBuffer.s));
        print(0, 0, "   Powered by   ", 16);
    }
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::cursor_pos(int row, int col) {
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::update(void) {
    int i = 0;
    for (uint8_t y = 0; y < 8; y++) {
        send_command(0xB0 + y);
        send_command(0x00);
        send_command(0x10);
        for (uint8_t x = 0; x < 128; x++) {
            send_data(displayBuffer.s[i++]);
        }
    }
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::send_command(uint8_t cmd) {
    i2c_buffer[0] = 0x00;
    i2c_buffer[1] = cmd;
    return (i2c_master_write_to_device(port, slave_address, i2c_buffer, 2, pdMS_TO_TICKS(LCD_IO_TIMEOUT_MS)));
}

esp_err_t DisplayOLED128x64::send_data(uint8_t data) {
    i2c_buffer[0] = 0x40;
    i2c_buffer[1] = data;
    return (i2c_master_write_to_device(port, slave_address, i2c_buffer, 2, pdMS_TO_TICKS(LCD_IO_TIMEOUT_MS)));
}

esp_err_t DisplayOLED128x64::print_large(int x, int y, const char* text, size_t length) {
    if (length == 0) {
        length = strlen(text);
    }
    for (int n = 0; n < (int)length; n++) {
        render_large_glyph(x + n * 2, y, text[n]);
    }
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::print(int x, int y, const char* text, size_t length) {
    if (length == 0) {
        length = strlen(text);
    }
    for (int n = 0; n < (int)length; n++) {
        render_glyph(x + n, y, text[n]);
    }
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::set_pixel(int x, int y, bool state) {
    if ((x < 0) || (x >= 128) || (y < 0) || (y >= 64)) {
        return (ESP_FAIL);
    }
    if (state) {
        displayBuffer.a[y / 8][x] |= (uint8_t)(1 << (y % 8));
    } else {
        displayBuffer.a[y / 8][x] &= (uint8_t)(0xff ^ (1 << (y % 8)));
    }
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::invert_row(int row) {
    int y = row * 2;
    if (swap_vertical) {
        y = (7 - y) ^ 0x01;
    }
    y &= 0x07;
    for (int x = 0; x < 128; x++) {
        displayBuffer.a[y + 0][x] ^= 0xff;
        displayBuffer.a[y + 1][x] ^= 0xff;
    }
    return (ESP_FAIL);
}

uint32_t DisplayOLED128x64::font_byte_16_to_32_bit(uint16_t fontWord) {
    uint32_t fontWord32 = 0;
    for (int i = 0; i < 16; i++) {
        if (fontWord & (1 << i)) {
            fontWord32 |= (uint32_t)(3 << (i * 2));
        }
    }
    return (fontWord32);
}

uint8_t DisplayOLED128x64::check_bit_swap(uint8_t fontData) {
    if (swap_vertical) {
        fontData = (bitReverseTab[(unsigned int)fontData]);
    }
    return (fontData);
}

uint16_t DisplayOLED128x64::check_bit_swap(uint16_t fontData) {
    if (swap_vertical) {
        uint8_t lsb = ((fontData >> 0) & 0xff);
        uint8_t msb = ((fontData >> 8) & 0xff);
        fontData = (bitReverseTab[(unsigned int)lsb] << 8) | (bitReverseTab[(unsigned int)msb]);
    }
    return (fontData);
}

int DisplayOLED128x64::check_line_swap8(int line) {
    if (swap_vertical) {
        line = (7 - line) ^ 0x01;
    }
    return (line & 0x07);
}

int DisplayOLED128x64::check_line_swap16(int line) {
    if (swap_vertical) {
        line = (7 - line) ^ 0x01;
    }
    return (line);
}

int DisplayOLED128x64::checkColumnSwap(int column) {
    if (swap_vertical) {
        column = 127 - column;
    }
    return (column & 0x7f);
}

esp_err_t DisplayOLED128x64::render_glyph(int x, int y, char glyphCode) {
    if ((x < 0) || (x >= 16) || (y < 0) || (y >= 8)) {
        return (ESP_FAIL);
    }

    TypeConvert tc{ 0 };

    const uint16_t* p = &font8x16_basic[(int)(glyphCode & 0x7f)][0];
    int col = x * 8;
    for (int n = 0; n < 8; n++) {
        tc.w[0] = check_bit_swap(p[n]);
        displayBuffer.a[check_line_swap8(y * 2 + 0)][checkColumnSwap(col + n)] = tc.b[0]; 
        displayBuffer.a[check_line_swap8(y * 2 + 1)][checkColumnSwap(col + n)] = tc.b[1]; 
    }

    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::render_large_glyph(int x, int y, char glyphCode) {
    if ((x < 0) || (x >= 15) || (y < 0) || (y >= 7)) {
        return (ESP_FAIL);
    }
    
    TypeConvert tc{ 0 };

    const uint16_t* p = &font8x16_basic[(int)(glyphCode & 0x7f)][0];
    int col = x * 8;
    for (int n = 0; n < 8; n++) {
        tc.dw = font_byte_16_to_32_bit(check_bit_swap(p[n]));
        int i = n * 2;

        if (swap_vertical) {
            displayBuffer.a[check_line_swap16(y * 2 + 2)][checkColumnSwap(col + i)] = tc.b[0];
            displayBuffer.a[check_line_swap16(y * 2 + 2)][checkColumnSwap(col + i + 1)] = tc.b[0];

            displayBuffer.a[check_line_swap16(y * 2 + 3)][checkColumnSwap(col + i)] = tc.b[1];
            displayBuffer.a[check_line_swap16(y * 2 + 3)][checkColumnSwap(col + i + 1)] = tc.b[1];

            displayBuffer.a[check_line_swap16(y * 2 + 0)][checkColumnSwap(col + i)] = tc.b[2];
            displayBuffer.a[check_line_swap16(y * 2 + 0)][checkColumnSwap(col + i + 1)] = tc.b[2];

            displayBuffer.a[check_line_swap16(y * 2 + 1)][checkColumnSwap(col + i)] = tc.b[3];
            displayBuffer.a[check_line_swap16(y * 2 + 1)][checkColumnSwap(col + i + 1)] = tc.b[3];
        } else {
            displayBuffer.a[check_line_swap16(y * 2 + 0)][checkColumnSwap(col + i)] = tc.b[0];
            displayBuffer.a[check_line_swap16(y * 2 + 0)][checkColumnSwap(col + i + 1)] = tc.b[0];

            displayBuffer.a[check_line_swap16(y * 2 + 1)][checkColumnSwap(col + i)] = tc.b[1];
            displayBuffer.a[check_line_swap16(y * 2 + 1)][checkColumnSwap(col + i + 1)] = tc.b[1];

            displayBuffer.a[check_line_swap16(y * 2 + 2)][checkColumnSwap(col + i)] = tc.b[2];
            displayBuffer.a[check_line_swap16(y * 2 + 2)][checkColumnSwap(col + i + 1)] = tc.b[2];

            displayBuffer.a[check_line_swap16(y * 2 + 3)][checkColumnSwap(col + i)] = tc.b[3];
            displayBuffer.a[check_line_swap16(y * 2 + 3)][checkColumnSwap(col + i + 1)] = tc.b[3];
        }
    }

    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::set_rotation(uint8_t mask) {
    swap_vertical = ((mask & 0x01) != 0);
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::set_contrast(float value) {
    uint8_t data = (uint8_t)(MAX(0.0f, MIN(1.0f, value)) * 255.0f);
    send_command(SSD1306_CMD_SET_CONTRAST);
    send_command(data);
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::off(void) {
    ESP_LOGI(TAG, "DisplayOLED128x64::off()");
    send_command(SSD1306_CMD_DISPLAY_OFF);
    return (ESP_OK);
}

esp_err_t DisplayOLED128x64::on(void) {
    ESP_LOGI(TAG, "DisplayOLED128x64::on()");
    send_command(SSD1306_CMD_DISPLAY_ON);
    return (ESP_OK);
}

