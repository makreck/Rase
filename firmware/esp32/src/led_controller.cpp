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

esp_err_t LEDController::init(void) {
    ledc_fade_func_install(0);

#ifdef _HAS_BOARD_LED
	boardLED = new BoardLED();
#endif

#ifdef _HAS_RGB_LED
    colorLED = new ColorLED();
#endif

#ifdef _HAS_NEOPIXEL_LED
    ws2812 = new WS2812(LED_NEOPIXEL, 1, NEOPIXEL_RMT_CHANNEL);
#endif

	set_LED(0.0f);
	set_color((color_t)ColorRef::WHITE, 0.0f);
    apply();
	
	return (ESP_OK);
}

esp_err_t LEDController::cleanup(void) {
	if (task_handle != nullptr) {
		vTaskDelete(task_handle);
		task_handle = nullptr;
	}

	SAFE_DELETE(ws2812);
    SAFE_DELETE(colorLED);
    SAFE_DELETE(boardLED);

	ledc_fade_func_uninstall();

	return (ESP_OK);
}

void LEDController::apply(void) {
	if (boardLED != nullptr) {
		boardLED->apply();
	}

	if (colorLED != nullptr) {
		colorLED->apply();
	}

	if (ws2812 != nullptr) {
		ws2812->apply();
	}
}

void LEDController::set_LED(float intensity) {
	if (boardLED != nullptr) {
		boardLED->set(intensity);
	}
}

void LEDController::set_color(const color_t color, float intensity) {
	currently_color.c = color;

	if (colorLED != nullptr) {
		colorLED->set(color, intensity);
	}

	if (ws2812 != nullptr) {
		ws2812->set(color, intensity);
	}
}

void LEDController::sequence(int count, int on_ms, int off_ms, float intensity) {
	if (task_handle != nullptr) {
		vTaskDelete(task_handle);
		task_handle = nullptr;
	}
	LEDSequenceParms* parms = new LEDSequenceParms(this, count, on_ms, off_ms, intensity);
    xTaskCreate(LEDController::_sequenceTask, "Sequence", TASK_DEFAULT_STACKSIZE, parms, TASK_DEFAULT_PRIORITY, &task_handle);
}

void LEDController::_sequenceTask(void* pvParameters) {
	LEDSequenceParms* parms = reinterpret_cast<LEDSequenceParms*>(pvParameters);
    (reinterpret_cast<LEDController*>(parms->p_this))->sequenceTask(parms);
}
void LEDController::sequenceTask(LEDSequenceParms* parms) {
    for (int i = 0; i < parms->count; i++) {
		set_LED(parms->intensity);
		set_color(currently_color.c, parms->intensity);
		apply();
		vTaskDelay(pdMS_TO_TICKS(parms->on_ms));

		set_LED(0.0f);
		set_color(currently_color.c, 0.0f);
		apply();
        vTaskDelay(pdMS_TO_TICKS(parms->off_ms));
    }

	delete (parms);
	TaskHandle_t h = task_handle;
	task_handle = nullptr;
	vTaskDelete(h);
}

bool LEDController::is_sequence_running(void) {
	return (task_handle != nullptr);
}

esp_err_t LEDController::config_LED_controller_timer(ledc_timer_t led_timer_num) {
	ledc_timer_config_t timerConfig;
	memset(&timerConfig, 0, sizeof(timerConfig));
	timerConfig.duty_resolution = LED_TIMER_PWM_RES;
	timerConfig.freq_hz 		= LED_TIMER_FREQ;
	timerConfig.clk_cfg 		= LED_TIMER_CLOCK_CFG;
	timerConfig.speed_mode 		= LED_TIMER_MODE;
	timerConfig.timer_num 		= led_timer_num;
	return (ledc_timer_config(&timerConfig));
}

esp_err_t LEDController::led_channel_config(gpio_num_t gpio_num, ledc_channel_t controller_channel, ledc_timer_t timer_num) {
    ledc_channel_config_t ledChannel;
    memset(&ledChannel, 0, sizeof (ledChannel));
	ledChannel.gpio_num   = gpio_num;
    ledChannel.channel    = controller_channel;
    ledChannel.timer_sel  = timer_num;
	ledChannel.speed_mode = LED_TIMER_MODE;
    ledChannel.intr_type  = LEDC_INTR_DISABLE;
    ledc_channel_config(&ledChannel);
	
	ledc_set_duty(LED_TIMER_MODE, controller_channel, 0);
	ledc_update_duty(LED_TIMER_MODE, controller_channel);
	ledc_set_fade_with_time(LED_TIMER_MODE, controller_channel, LED_TIMER_PWM_MAX, LED_FADING_TIME_MS);
	ledc_fade_start(LED_TIMER_MODE, controller_channel, LEDC_FADE_NO_WAIT);

	return (ESP_OK);
}
