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

#define BTN_IDLE                   (0)
#define BTN_SHORT_PRESS            (1)
#define BTN_MEDIUM_PRESS           (2)
#define BTN_LONG_PRESS             (3)

#define BTN_QUICK_RELEASE_MS       (180)
#define BTN_SWITCH_TIME_MS         (1000)
#define BTN_STEP_TIME_MS           (250)
#define BTN_MIN_INACTIVITY_TIME_MS (300)
#define BTN_LONG_PRESS_TIME_MS     (5000)
#define BTN_BOUNCE_MS              (5)

class ButtonState {
    public:
        int        button_message   = 0;
        int        press_count      = 0;
        TickType_t time_pressed_ms  = 0;
        TickType_t time_released_ms = 0;
        TickType_t time_idle_ms     = 0;

        void set(ButtonState* source) {
            if (source != nullptr) {
                memcpy(this, source, sizeof (ButtonState));
            } else {
                memset(this, 0, sizeof (ButtonState));
            }
        }
};

class Button {
    private:
        static const constexpr gpio_num_t button_port[3] = { ESP32_BOOT_BUTTON, ESP32_ALT_BUTTON1, ESP32_ALT_BUTTON2 };

        ButtonState  state;
        TaskHandle_t task_handle        = nullptr;
        TickType_t   time_stamp_ms      = 0;
        TickType_t   time_elapsed_ms    = 0;
        bool         new_state          = false;
        bool         button_state       = false;
        int          state_change_count = 0;
        int          last_change_count  = 0;

    public:
        Button(void) {
            init();
        }

        ~Button() {
            cleanup();
        }

        static void _buttonTask(void* pvParameters);
        void buttonTask(void);

        void init();
        void cleanup(void);
        bool get_state(void);
        void interpret_state(void);
        void handle_event_time(void);
        void handle_event_state(void);
        void handle_idle_time(void);
        void event_loop(void);
};
