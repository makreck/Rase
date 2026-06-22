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

// #define DISPLAY_STATE

AppState App::init_display(void) {
#ifdef _ENABLE_LCD
    m.display = new DisplayI2C();
    if (m.display != nullptr) {
        m.display_page     = DisplayPage::invalid;
        m.display_request  = 1;
        m.display_update   = 0;
        reload_display_timeout();

        m.display->clear();
        m.display->set_contrast(m.cfg->get_display_contrast());
        m.display->set_rotation(m.cfg->get_rotation());
        m.display->setLogo();
        m.display->update();
        
        return (AppState::OK);
    }
    return (AppState::failed);
#else
    return (AppState::not_implemented);
#endif    
}

AppState App::handle_display(void) {
    if (m.display == nullptr) {
        return (AppState::not_implemented);
    }

    if (m.display_page == DisplayPage::invalid) {
        m.display_page = DisplayPage::value_page;
        m.display_request++;
    } else if (m.display_page == DisplayPage::menu) {
        reload_screensaver();
        return (AppState::menu);
    } 

    if (m.flags.b.bButtonEvent == 1) {
        if (reload_screensaver() == AppState::idle) {
            m.flags.b.bButtonEvent = 0;    
            m.display_request++;
        } else {
            if (m.btnState.button_message == BTN_SHORT_PRESS) {
                m.flags.b.bButtonEvent = 0;    
                display_next();
            }
        }
    } else {
        if (check_screensaver() == AppState::sleep) {
            return (AppState::sleep);
        }
    }

    if (m.display_request <= m.display_update) {
        return (AppState::idle);
    }
    m.display_update = m.display_request;
    m.display->clear();

    switch (m.display_page) {
        case DisplayPage::title_screen: {
            m.display->setLogo();
        } break;

        case DisplayPage::value_page: {
            display_value_page();
        } break;

        default: {
            m.display_page = DisplayPage::value_page;
            display_value_page();
        } break;
    }

    m.display->update();

    return (AppState::OK);
}

AppState App::print_net_Info(void) {
    const char* ip_txt;
    if (m.flags.b.bWifiConnected == 1) {
        ip_txt = m.station->get_ip();
    } else if (m.flags.b.bWifiEnabled == 1) {
        ip_txt = "WLAN connecting";
    } else {
        ip_txt = "WLAN not connect";
    }
    m.display->print(0, 3, ip_txt, strlen(ip_txt));
    return (AppState::OK);
}

AppState App::print_measuring_info(uint8_t mode) {
    char line[17]{' '};
    
    if (m.driver != nullptr) {
        const char* head = m.driver->get_head();
        memcpy(&line[0], head, strlen(head));

        if (mode == 1) {
            snprintf(&line[8], (size_t)9, "%-8.8X", (unsigned int)m.driver->get_head_serial_number());
        }
    }

    if (mode == 0) {
        Tools::get_time(&line[8], 9);
    }

    m.display->print(0, 4, line, 16);
    return (AppState::OK);
}

AppState App::print_large_text(int start) {
    SensorNodeProps props{0};
    char string[20]{0};
    int count = (int)m.sensor->get_channel_count();
    int x = 0;
    for (int i = 0; i < std::min(2, count); i++) {
        if (!m.sensor->get_channel_props((start + (int)i) % count, props)) break;
        snprintf(string, sizeof (string), "%.1f", props.value);
        char *p1 = string;
        char *p2 = strstr(p1, ".");
        if (p2 != nullptr) {
            *p2 = 0;
        }
        int len = strlen(p1);
        int x2 = x + len + len;
        m.display->print_large(x, 1, p1, len);
        if (p2 != nullptr) {
            *p2 = '.';
            m.display->print(x2, 2, p2, strlen(p2));
        }
        m.display->print(x2 + 1, 1, props.unit, MIN(3, strlen(props.unit)));
        x += 8;
        if (x2 > 7) break;
    }

    return (AppState::OK);
}

void App::display_next(void) {
    int count = (int)m.sensor->get_channel_count();
    int index = (int)m.cfg->get_display_parameter();
    index = (index + 1) % count;
    m.cfg->set_display_parameter((uint8_t)(index & 0xff));
}

AppState App::display_small_value_page(void) {
    char line[17]{' '};
    Tools::get_time(&line[0], 9);
    if (m.driver != nullptr) {
        const char* head = m.driver->get_head();
        memcpy(&line[9], head, strlen(head));
    }
    for (int i = 0; i < 16; i++) {
        if (line[i] < 32) line[i] = ' ';
    }
    line[16] = 0;
    m.display->print(0, 0, line, 16);

    const char* ip_txt;
    if (m.flags.b.bWifiConnected == 1) {
        ip_txt = m.station->get_ip();
    } else if (m.flags.b.bWifiEnabled == 1) {
        ip_txt = "WLAN connecting";
    } else {
        ip_txt = "WLAN not connect";
    }
    m.display->print(0, 1, ip_txt, strlen(ip_txt));

    return (AppState::OK);
}

AppState App::display_value_page(void) {
    DisplayLayout layout_type = m.cfg->get_display_layout();

    if (m.display->has_DisplayOfType(DisplayType::OLED128x64)) {
        switch (layout_type) {
            case DisplayLayout::info: {
                display_info_page();
            } break;

            case DisplayLayout::detailes: {
                display_details_page();
            } break;

            case DisplayLayout::large_values:
            default: {
                print_measuring_info();
                print_large_text(m.cfg->get_display_parameter());
                print_net_Info();
            } break;
        }
    }
        
    if (m.display->has_DisplayOfType(DisplayType::LCD16x2)) {
        switch (layout_type) {
            case DisplayLayout::info:
                // No layout available, display space is too small!
            case DisplayLayout::detailes:
                // No layout available, display space is too small!
            case DisplayLayout::large_values:
            default: {
                display_small_value_page();

            } break;
        }
    }

    return (AppState::OK);
}

AppState App::display_details_page(void) {
    SensorNodeProps props{0};
    char string[20]{0};
    int count = (int)m.sensor->get_channel_count();
    int x = 0;
    int y = 0;

    switch (count) {
        case 0: {
            print_measuring_info(1);
            if (m.sensor->get_channel_props(0, props)) {
                m.display->print_large(x, 1, "Sensor?");
            }
            print_net_Info();
        } break;

        case 1: {
            print_measuring_info(1);
            if (m.sensor->get_channel_props(0, props)) {
                snprintf(string, sizeof (string), "%-4.1f%s", props.value, props.unit);
                string[8] = 0;
                m.display->print_large(x, 1, string);
            }
            print_net_Info();
        } break;

        case 2:
        case 3: {
            print_measuring_info(1);
            y = 1;
            for (int i = 0; i < count; i++) {
                if (m.sensor->get_channel_props(i, props, false)) {
                    snprintf(string, sizeof (string), "%2.2s = %-4.1f %s", props.shortcut, props.value, props.unit);
                    string[16] = 0;
                    m.display->print(x, y, string);
                }
                y++;
            }
            if (y < 4) {
                print_net_Info();
            }
        } break;

        case 4: {
            y = 0;
            for (int i = 0; i < count; i++) {
                if (m.sensor->get_channel_props(i, props, false)) {
                    snprintf(string, sizeof (string), "%2.2s = %-4.1f %s", props.shortcut, props.value, props.unit);
                    string[16] = 0;
                    m.display->print(x, y, string);
                }
                y++;
            }
        } break;

        default: {
            print_measuring_info(1);
            y = 1;
            for (int i = 0; i < count; i++) {
                if (m.sensor->get_channel_props(i, props)) {
                    snprintf(string, sizeof (string), "%2.2s=%.1f", props.shortcut, props.value);
                    string[7] = ' ';
                    m.display->print(x, y, string);
                    x += 8; if (x >= 16) { x = 0; y++; }
                }
            }
            if (y < 3) {
                print_net_Info();
            }
        } break;
    }

    return (AppState::OK);
}

AppState App::display_info_page(void) {
    SensorNodeProps props{0};
    char string[20]{0};
    int count = (int)m.sensor->get_channel_count();

    print_measuring_info(1);

    for (int i = 0; (i < count) && (strlen(string) < 16); i++) {
        if (m.sensor->get_channel_props(i, props)) {
            if (i > 0) strcat(string, "/");
            strcat(string, props.shortcut);
        }
    }
    string[16] = 0;
    m.display->print(0, 1, string);

    Tools::get_timestamp(string, sizeof (string), ' ');
    string[16] = 0;
    m.display->print(0, 2, string);
    
    print_net_Info();

    return (AppState::OK);
}

void App::flip_display(void) {
    m.cfg->flip_Rotation();
    m.display->set_rotation(m.cfg->get_rotation());
    m.display_request++;
}

void App::set_display_contrast(float value) {
    if (m.display != nullptr) {
        m.display->set_contrast(value);
        m.display_request++;
    }

    m.cfg->set_display_contrast(value);
    request_sys_config_update();
}

AppState App::set_display_page(DisplayPage page) {
    m.display_page = page;
    m.display_request++;
    return (AppState::OK);
}

bool App::reload_display_timeout(void) {
    uint32_t reload = (uint32_t)m.cfg->get_display_timeout();
    if (reload != 0) {
        uint32_t tick_time_s = Tools::get_tick_seconds();
#ifdef DISPLAY_STATE
        if (m.display_off_time != (tick_time_s + reload)) {
            ESP_LOGI(TAG, "Display timeout reload for %us, time=%u, threshold=%u",
                (unsigned int)reload, (unsigned int)tick_time_s, (unsigned int)m.display_off_time);
        }
#endif
        m.display_off_time = tick_time_s + reload;
        return (true);
    } 
    m.display_off_time = 0;
    return (false);
}

AppState App::reload_screensaver(void) {
    AppState state = AppState::OK;

    if (m.flags.b.bDisplayOFF == 1) {
        m.flags.b.bDisplayOFF = 0;
        m.display->on();
        state = AppState::idle;
    }

    reload_display_timeout();
    return (state);
}

AppState App::check_screensaver(void) {
    if ((m.display_off_time == 0) || (m.flags.b.bDisplayOFF == 1)) {
        return (AppState::OK);
    }

    if (Tools::get_tick_seconds() >= m.display_off_time) {
        if (m.flags.b.bDisplayOFF == 0) {
            m.display->off();
            m.flags.b.bDisplayOFF = 1;
        }
        return (AppState::sleep);
    }

    return (AppState::OK);
}
