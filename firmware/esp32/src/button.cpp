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

#define DISPLAY_STATE

void Button::init(void) {
    for (size_t i = 0; i < SIZEOFARRAY(button_port); i++) {
        if (button_port[i] != GPIO_NUM_NC) {
            gpio_reset_pin(button_port[i]);
            gpio_set_direction(button_port[i], GPIO_MODE_INPUT);
            gpio_pulldown_dis(button_port[i]);
            gpio_pullup_en(button_port[i]);
        }
    }
    last_change_count = state_change_count;
    xTaskCreate(Button::_buttonTask, "Button", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY, &task_handle);
}

void Button::cleanup(void) {
    vTaskDelete(task_handle);
    for (size_t i = 0; i < SIZEOFARRAY(button_port); i++) {
        if (button_port[i] != GPIO_NUM_NC) {
            gpio_reset_pin(button_port[i]);
        }
    }
}

bool Button::get_state(void) {
    return (button_state);
}

void Button::handle_event_time(void) {
    TickType_t time_ms = pdTICKS_TO_MS(xTaskGetTickCountFromISR());
    if (new_state) {
        time_stamp_ms = time_ms;
    } else {
        time_elapsed_ms = time_ms - time_stamp_ms;
    }
}

void Button::handle_event_state(void) {
    if (new_state) {
        state.time_released_ms = time_elapsed_ms;
    } else {
        state.time_pressed_ms = time_elapsed_ms;
        if ((state.time_pressed_ms < BTN_QUICK_RELEASE_MS) && (state.time_released_ms < BTN_QUICK_RELEASE_MS)) {
            state.press_count++;
        } else {
            state.press_count = 1;
        }
        state_change_count++;
    }
}

void Button::interpret_state(void) {
    if (state.time_pressed_ms >= BTN_SWITCH_TIME_MS) {
        state.button_message = BTN_LONG_PRESS;
    } else if (state.time_pressed_ms <= BTN_STEP_TIME_MS) {
        state.button_message = BTN_SHORT_PRESS;
    } else {
        state.button_message = BTN_MEDIUM_PRESS;
    }
}

void Button::handle_idle_time(void) {
    TickType_t time_ms = pdTICKS_TO_MS(xTaskGetTickCountFromISR());
    state.time_idle_ms = time_ms - time_stamp_ms;
    if (state.time_idle_ms >= BTN_MIN_INACTIVITY_TIME_MS) {
        if (state.press_count > 0) {
            state.press_count = 0;
            state.button_message = BTN_IDLE;
            esp_event_post(APP_EVENT, (int32_t)AppEvent::button_idle, &state, sizeof (state), pdMS_TO_TICKS(10));
        }
    }
}

void Button::event_loop(void) {
    bool pinState = false;
    for (int i = 0; i < SIZEOFARRAY(button_port); i++) {
        if (button_port[i] != GPIO_NUM_NC) {
            if (gpio_get_level(button_port[i]) == 0) {
                pinState = true;
            }
        }
    }
    new_state = pinState;
    if (new_state != button_state) {
        button_state = new_state;
        handle_event_time();
        handle_event_state();
        if (last_change_count != state_change_count) {
            state_change_count = last_change_count;
            interpret_state();
            esp_event_post(APP_EVENT, (int32_t)AppEvent::button_event, &state, sizeof (state), pdMS_TO_TICKS(10));
        }
    }

    if (new_state == 0) {
        handle_idle_time();
    }
}

void Button::_buttonTask(void *pvParameters) {
    (reinterpret_cast<Button *>(pvParameters))->buttonTask();
}
void Button::buttonTask(void) {
    esp_event_post(APP_EVENT, (int32_t)AppEvent::button_ready, &state, sizeof (state), pdMS_TO_TICKS(10));
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10));
        event_loop();
    }
}
