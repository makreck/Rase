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

#define LED_TIMER_FREQ          (8192)
#define LED_TIMER_CLOCK_CFG     (LEDC_AUTO_CLK)
#define LED_TIMER_MODE          (LEDC_LOW_SPEED_MODE)
#define LED_TIMER_PWM_RES       (LEDC_TIMER_12_BIT)
#define LED_TIMER_PWM_MAX       ((1 << LED_TIMER_PWM_RES) - 2)
#define LED_FADING_TIME_MS      (100)
#define LED_BOARD_CHANNEL       LEDC_CHANNEL_3
#define LED_BOARD_TIMER         LEDC_TIMER_3

#define LED_RGB_TIMER           (LEDC_TIMER_0)
#define RGBLED_R                (0)
#define RGBLED_G                (1)
#define RGBLED_B                (2)
#define RGBLED_MSK_R            (1 << RGBLED_R)
#define RGBLED_MSK_G            (1 << RGBLED_G)
#define RGBLED_MSK_B            (1 << RGBLED_B)
#define RGBLED_MSK_WHITE        (RGBLED_MSK_R | RGBLED_MSK_G | RGBLED_MSK_B)

#define NEOPIXEL_RMT_CHANNEL    (RMT_CHANNEL_0)
#define NEOPIXEL_WS2812_CLK_DIV (8)


class BoardLED {
    private:
        uint32_t duty_cycle = 0;
        float factor = 0.99f;
        float offset = 0.01f;
        bool modified = true;

    public:
        BoardLED(void) {
            init();
        }

        ~BoardLED() {
            cleanup();
        }

        void init(void);
        void cleanup();
        void set(float intensity);
        void apply(void);
};


class ColorLED {
    private:
        uint32_t led_state = 0;
        int count_of_channels = 0;
        float intensity[3]{ 1.0f, 1.0f, 1.0f };
        gpio_num_t pin[3] = { RGBLED_PIN_R, RGBLED_PIN_G, RGBLED_PIN_B };

    public:
        ColorLED(gpio_num_t pinR = RGBLED_PIN_R, gpio_num_t pinG = RGBLED_PIN_G, gpio_num_t pinB = RGBLED_PIN_B) {
            init(pinR, pinG, pinB);
        }

        ~ColorLED() {
            cleanup();
        }

        void init(gpio_num_t givenPinR, gpio_num_t givenPinG, gpio_num_t givenPinB);
        void cleanup(void);
        
        void set(Color color, float led_intensity = 1.0f);
        void apply(void);

};


class NeoPixelItem : public rmt_item32_t {
    public:
        void setHigh(void) {
            level0    = 1;
            duration0 = 10;
            level1    = 0;
            duration1 = 6;
        }
        
        void setLow(void) {
            level0    = 1;
            duration0 = 4;
            level1    = 0;
            duration1 = 8;
        }
        
        void setTerm(void) {
            level0    = 0;
            duration0 = 0;
            level1    = 0;
            duration1 = 0;
        }
};


class WS2812 {
    private:
        size_t         pixelCount = 1;
        size_t         itemCount  = 25;
        rmt_channel_t  channel    = NEOPIXEL_RMT_CHANNEL;
        Color*         pixels     = nullptr;
        NeoPixelItem*  items      = nullptr;
        bool           modified   = true;

    public:
        WS2812(gpio_num_t gpioNum = LED_NEOPIXEL, int pixelCount = 1, rmt_channel_t channel = NEOPIXEL_RMT_CHANNEL) {
            init(gpioNum, pixelCount, channel);
        }

        ~WS2812() {
            cleanup();
        }

        static void translate(Color& nativeColor, Color color, float intensity);

        void init(gpio_num_t gpioNum, int pixelCount, rmt_channel_t channel);
        void cleanup(void);

        void set(Color color, float led_intensity = 1.0f, int index = -1);
        void apply();

        size_t get_count(void);
        void set_pixel(int index, Color pixel);
        void set_all(Color pixel);
        void clear();

};

class LEDSequenceParms {
    public:
        void* p_this    = nullptr;
        int   count     = 1;
        int   on_ms     = 50;
        int   off_ms    = 450;
        float intensity = 1.0f;

        LEDSequenceParms(void* _pThis, int _count, int _on_ms, int _off_ms, float _intensity = 1.0f) {
            p_this    = _pThis;
            count     = _count;
            on_ms     = _on_ms;
            off_ms    = _off_ms;
            intensity = _intensity;
        }
};

class LEDController {
    private:
        Color        currently_color;
        ColorLED*    colorLED    = nullptr;
        WS2812*      ws2812      = nullptr;
        BoardLED*    boardLED    = nullptr;
        TaskHandle_t task_handle = nullptr;

        static void _sequenceTask(void* pvParameters);
        void sequenceTask(LEDSequenceParms* parms);
        
    public:
        LEDController() {
            init();
        }

        ~LEDController() {
            cleanup();
        }

        esp_err_t init(void);
        esp_err_t cleanup(void);

        static esp_err_t config_LED_controller_timer(ledc_timer_t led_timer_num);
        static esp_err_t led_channel_config(gpio_num_t gpio_num, ledc_channel_t controller_channel, ledc_timer_t timer_num);

        void sequence(int count, int on_ms, int off_ms, float intensity = 1.0f);
        bool is_sequence_running(void);
        void set_LED(float intensity);
        void set_color(const color_t color, float intensity = 1.0f);
        void apply(void);
};
