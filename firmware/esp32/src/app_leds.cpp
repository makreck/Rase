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

AppState App::init_LEDs(void) {
#if defined _ENABLE_BOARD_LED || defined _HAS_NEOPIXEL_LED || defined _HAS_BOARD_LED
    m.led = new LEDController();
    AppState state = ((m.led != nullptr) ? AppState::OK : AppState::failed);
    return (state);
#else
    return (AppState::not_implemented);
#endif    
}

AppState App::handle_LEDs(void) {
    if (m.led == nullptr) {
        return (AppState::not_implemented);
    }

    float intensity = m.cfg->get_LED_intensity();

    if (m.flags.b.bError == 1) {
        m.led->set_color((color_t)ColorRef::RED, intensity);
        m.led->set_LED(intensity);
    } else if (m.flags.b.bDriverQuery == 1) {
        m.flags.b.bDriverQuery = 0;
        m.led->set_color((color_t)ColorRef::GREEN, intensity);
        m.led->set_LED(intensity);
    } else if ((m.flags.b.bWebsiteQuery == 1) || (m.flags.b.bWebAPIQuery == 1)) {
        m.flags.b.bWebsiteQuery = 0;
        m.flags.b.bWebAPIQuery = 0;
        m.led->set_color((color_t)ColorRef::BLUE, intensity);
        m.led->set_LED(intensity);
    } else {
        m.led->set_LED(0.0f);
        if (m.flags.b.bDriverReady == 0) {
            m.led->set_color((color_t)ColorRef::RED, intensity);
        } else if (m.flags.b.bWifiConnected == 0) {
            m.led->set_color((color_t)ColorRef::ORANGE, intensity);
        } else if (m.flags.b.bWebsiteReady == 0) {
            m.led->set_color((color_t)ColorRef::YELLOW, intensity);
        } else {
            m.led->set_color((color_t)ColorRef::BLACK, 0.0f);
        }
    }

    m.led->apply();

    return (AppState::OK);
}
