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

void BoardLED::init(void) {
    factor = LED_TIMER_PWM_MAX * 0.99f;
    offset = LED_TIMER_PWM_MAX - factor;

    gpio_reset_pin(LED_DEVBOARD_PIN);
    gpio_set_direction(LED_DEVBOARD_PIN, GPIO_MODE_OUTPUT);
    LEDController::config_LED_controller_timer(LED_BOARD_TIMER);
    LEDController::led_channel_config(LED_DEVBOARD_PIN, LED_BOARD_CHANNEL, LED_BOARD_TIMER);
}

void BoardLED::cleanup() {
    set(0.0f);
    apply();
    ledc_stop(LED_TIMER_MODE, LED_BOARD_CHANNEL, 0);
    gpio_reset_pin(LED_DEVBOARD_PIN);
}

void BoardLED::set(float intensity) {
    if (intensity > 0.0f) {
        float f = MAX(0.0f, MIN(1.0f, intensity));
        uint32_t cycle = (uint32_t)(f * factor + offset);
        modified |= (cycle != duty_cycle);
        duty_cycle = cycle;
    } else {
        modified |= (duty_cycle != 0);
        duty_cycle = 0;
    }
}

void BoardLED::apply(void) {
    if (modified) {
        modified = false;
#ifdef DISPLAY_STATE
        ESP_LOGI(TAG, "BoardLED::apply() = 0x%-2.2X  ", (unsigned int)duty_cycle);
#endif
        // ledc_set_duty(LED_TIMER_MODE, LED_BOARD_CHANNEL, duty_cycle);
        // ledc_update_duty(LED_TIMER_MODE, LED_BOARD_CHANNEL);

#ifdef _BOARD_LED_INVERTED
        uint32_t pwm_duty_cycle = 1 + LED_TIMER_PWM_MAX - duty_cycle;
#else        
        uint32_t pwm_duty_cycle = duty_cycle;
#endif        
        ledc_set_fade_with_time(LED_TIMER_MODE, LED_BOARD_CHANNEL, pwm_duty_cycle, LED_FADING_TIME_MS);
        ledc_fade_start(LED_TIMER_MODE, LED_BOARD_CHANNEL, LEDC_FADE_NO_WAIT);
    }
}
