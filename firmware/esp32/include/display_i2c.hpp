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

#pragma once

#define DISPLAY_I2C (I2C_NUM_1)
//#define LCD_SLAVE_ADDRESS (0x3F) // (0x27) // 0x20 ... 0x27

#define LCD16X2_A0 0x20
#define LCD16X2_A1 0x21
#define LCD16X2_A2 0x22
#define LCD16X2_A3 0x23
#define LCD16X2_A4 0x24
#define LCD16X2_A5 0x25
#define LCD16X2_A6 0x26
#define LCD16X2_A7 0x27
#define LCD16X2_A8 0x3F

#define OLED_128X64_A0 (0x3C)

#define LCD_IO_TIMEOUT_MS (40)
#define LCD_IIC_FREQ_HZ (400000)

enum class DisplayType : uint8_t {
    undefined = 0,
    LCD16x2 = 1,
    OLED128x64 = 2,
};

class DisplayInstance {
    public:
        DisplayInstance* next = nullptr;

        DisplayType type = DisplayType::undefined;
        i2c_port_t port = DISPLAY_I2C;
        uint8_t slave_address = 0x00;
        bool f_available = false;
        bool swap_vertical = false;

        DisplayInstance(i2c_port_t _given_port, DisplayType _given_type, uint8_t _given_address, uint8_t _given_rotation_mask) {
            port          = _given_port;
            type          = _given_type;
            slave_address = _given_address;
            swap_vertical = ((_given_rotation_mask & 0x01) != 0);
        }

        virtual esp_err_t init(void) = 0;
        virtual esp_err_t clear(void) = 0;
        virtual esp_err_t setLogo(void) = 0;
        virtual esp_err_t cursor_pos(int row, int col) = 0;
        virtual esp_err_t update(void) = 0;
        virtual esp_err_t print(int x, int y, const char* text, size_t length = 0) = 0;
        virtual esp_err_t print_large(int x, int y, const char* text, size_t length = 0) = 0;
        virtual esp_err_t invert_row(int row) = 0;
        virtual esp_err_t set_rotation(uint8_t mask) = 0;
        virtual esp_err_t set_contrast(float value) = 0;
        virtual esp_err_t off(void) = 0;
        virtual esp_err_t on(void) = 0;
};

class DisplayLCD16x2 : public DisplayInstance {
    private:
        union {
            char s[32];
            char a[2][16];
        } displayBuffer;

    public:
        DisplayLCD16x2(i2c_port_t _given_port, DisplayType _given_type, uint8_t _given_address, uint8_t _given_rotation_mask) : DisplayInstance(_given_port, _given_type, _given_address, _given_rotation_mask) {
        }

        esp_err_t send_command(uint8_t cmd);
        esp_err_t send_data(char data);

        esp_err_t init(void) override;
        esp_err_t clear(void) override;
        esp_err_t setLogo(void) override;
        esp_err_t cursor_pos(int row, int col) override;
        esp_err_t update(void) override;
        esp_err_t print(int x, int y, const char* text, size_t length = 0) override;
        esp_err_t print_large(int x, int y, const char* text, size_t length = 0) override;
        esp_err_t invert_row(int row) override;
        esp_err_t set_rotation(uint8_t mask) override;
        esp_err_t set_contrast(float value) override;
        esp_err_t off(void) override;
        esp_err_t on(void) override;
};

class DisplayOLED128x64 : public DisplayInstance {
    private:
        static constexpr const char* CLASS_ID = "DisplayOLED128x64";

        uint8_t i2c_buffer[4]{ 0 };
        union {
            uint8_t s[8 * 128]; // 1024
            uint8_t a[8][128];
        } displayBuffer;

        uint32_t font_byte_16_to_32_bit(uint16_t fontWord);
        uint8_t check_bit_swap(uint8_t fontData);
        uint16_t check_bit_swap(uint16_t fontData);
        int check_line_swap8(int line);
        int check_line_swap16(int line);
        int checkColumnSwap(int column);

    public:
        DisplayOLED128x64(i2c_port_t _given_port, DisplayType _given_type, uint8_t _given_address, uint8_t _given_rotation_mask) : DisplayInstance(_given_port, _given_type, _given_address, _given_rotation_mask) {
        }

        esp_err_t send_command(uint8_t cmd);
        esp_err_t send_data(uint8_t data);

        esp_err_t set_pixel(int x, int y, bool state);
        esp_err_t render_glyph(int x, int y, char glyphCode);
        esp_err_t render_large_glyph(int x, int y, char glyphCode);

        esp_err_t init(void) override;
        esp_err_t clear(void) override;
        esp_err_t setLogo(void) override;
        esp_err_t cursor_pos(int row, int col) override;
        esp_err_t update(void) override;
        esp_err_t print(int x, int y, const char* text, size_t length) override;
        esp_err_t print_large(int x, int y, const char* text, size_t length) override;
        esp_err_t invert_row(int row) override;
        esp_err_t set_rotation(uint8_t mask) override;
        esp_err_t set_contrast(float value) override;
        esp_err_t off(void) override;
        esp_err_t on(void) override;
};
typedef DisplayInstance* DisplayHandle;

class DisplayI2C {
    private:
        static constexpr const char* CLASS_ID = "DisplayI2C";

        i2c_port_t port = DISPLAY_I2C;
        gpio_num_t sda  = DISPLAY_I2C_SDA;
        gpio_num_t scl  = DISPLAY_I2C_SCL;

        int displayCount = 0;
        DisplayInstance* control = nullptr;
        QueueHandle_t mutex = nullptr;
        
    public:

        DisplayI2C(i2c_port_t lcdPort = DISPLAY_I2C, gpio_num_t pinSDA = DISPLAY_I2C_SDA, gpio_num_t pinSCL = DISPLAY_I2C_SCL) {
            init(lcdPort, pinSDA, pinSCL);
        }

        ~DisplayI2C() {
            cleanup();
        }

        void init(i2c_port_t lcdPort, gpio_num_t pinSDA, gpio_num_t pinSCL);
        void cleanup(void);
        void portSetup(void);
        bool isAvailable(void) { return (displayCount > 0); }
        bool has_DisplayOfType(DisplayType type);

        esp_err_t detect_display_type(void);
        DisplayHandle create_display(DisplayType type, uint8_t i2Caddr, uint8_t rotationMask);

        esp_err_t clear(void);
        esp_err_t setLogo(void);
        esp_err_t set_rotation(uint8_t mask);
        esp_err_t set_contrast(float value);
        esp_err_t print(int x, int y, const char* text, size_t length = 0);
        esp_err_t print_large(int x, int y, const char* text, size_t length = 0);
        esp_err_t cursor_pos(int row, int col);
        esp_err_t invert_row(int row);
        esp_err_t update(void);
        esp_err_t off(void);
        esp_err_t on(void);
};

extern const uint8_t font8x8_basic[128][8];
extern const uint16_t font8x16_basic[128][8];
extern const uint8_t kks_logo_image[1024];
extern const uint8_t bitReverseTab[256];
