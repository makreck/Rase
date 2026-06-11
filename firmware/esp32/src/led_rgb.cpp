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

void ColorLED::init(gpio_num_t pinR, gpio_num_t pinG, gpio_num_t pinB) {
    pin[RGBLED_R] = pinR;
    pin[RGBLED_G] = pinG;
    pin[RGBLED_B] = pinB;

    LEDController::config_LED_controller_timer(LED_RGB_TIMER);

    count_of_channels = 0;
    for (int i = 0; i < SIZEOFARRAY(pin); i++) {
        if (pin[i] != GPIO_NUM_NC) {
            count_of_channels++;
            gpio_reset_pin(pin[i]);
            gpio_set_direction(pin[i], GPIO_MODE_OUTPUT);
        }
    }

    for (int i = 0; i < SIZEOFARRAY(pin); i++) {
        if (pin[i] != GPIO_NUM_NC) {
            LEDController::led_channel_config(pin[i], (ledc_channel_t)(i), LED_RGB_TIMER);
        }
    }
}

void ColorLED::cleanup(void) {
    for (int i = 0; i < SIZEOFARRAY(pin); i++) {
        if (pin[i] != GPIO_NUM_NC) {
            ledc_channel_t chn = (ledc_channel_t)(i);
            ledc_stop(LED_TIMER_MODE, chn, 0);
            gpio_reset_pin(pin[i]);
        }
    }
}

void ColorLED::set(Color color, float led_intensity) {
    intensity[0] = color.get_red()   * led_intensity;
    intensity[1] = color.get_green() * led_intensity;
    intensity[2] = color.get_blue()  * led_intensity;
    led_state = RGBLED_MSK_WHITE;
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "ColorLED::set(color=0x%-8.8X, intensity=%.3f) = %.1f %.1f %.1f",
        (unsigned int)color.c, led_intensity, intensity[0], intensity[1], intensity[2]);
#endif
}

void ColorLED::apply(void) {
    float pwmMax = LED_TIMER_PWM_MAX;
    for (int i = 0; i < SIZEOFARRAY(pin); i++) {
        if (pin[i] != GPIO_NUM_NC) {
            float duty = LED_TIMER_PWM_MAX - MAX(0.0f, MIN(pwmMax, ((((led_state & (1 << i)) != 0) ? pwmMax : 0.0f) * intensity[i])));
            ledc_channel_t chn = (ledc_channel_t)(i);
            ledc_set_fade_with_time(LED_TIMER_MODE, chn, (uint32_t)(duty), LED_FADING_TIME_MS);
            ledc_fade_start(LED_TIMER_MODE, chn, LEDC_FADE_NO_WAIT);
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "ColorLED::apply() -> channel = %d, duty = %d/%d", (int)chn, (unsigned int)duty, (unsigned int)pwmMax);
#endif
        }
    }
}
