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

const MenuItem menu_main[] {
    IDM_TITLE,               "Main menu",
    IDM_EXIT,                "1 Exit",
    IDM_LAYOUT,              "2 Layout",
    IDM_DISPLAY,             "3 Display",
    IDM_LED_INTENSITY,       "4 LED intensity",
    IDM_REBOOT,              "5 Reboot system",
    IDM_FACTORY_RESET,       "6 Factory reset",
    IDM_CONFIG,              "7 Setup",
};
const size_t menu_main_size = SIZEOFARRAY(menu_main); 

const MenuItem menu_display[] {
    IDM_TITLE,               "Display settings",
    IDM_MAIN,                "1 Main menu",
    IDM_ROTATE,              "2 Rotate",
    IDM_CONTRAST,            "3 Contrast",
    IDM_DISPLAY_OFF,         "4 Timeout",
};
const size_t menu_display_size = SIZEOFARRAY(menu_display); 

const MenuItem menu_config[] {
    IDM_TITLE,               "Display contrast",
    IDM_MAIN,                "1 Main menu",
    IDM_CONFIG_INTERFACE,    "2 COM on/off",
    IDM_SENSOR_SELECT,       "3 Sensor select",
};
const size_t menu_config_size = SIZEOFARRAY(menu_config); 

const MenuItem menu_contrast[] {
    IDM_TITLE,               "Display contrast",
    IDM_MAIN,                "1 Main menu",
    IDM_CONTRAST_HIGH,       "2 High",
    IDM_CONTRAST_MEDIUM,     "3 Medium",
    IDM_CONTRAST_LOW,        "4 Low",
};
const size_t menu_contrast_size = SIZEOFARRAY(menu_contrast); 

const MenuItem menu_layout[] {
    IDM_TITLE,               "Screen layout",
    IDM_MAIN,                "1 Main menu",
    IDM_LAYOUT_VALUE_PAGE,   "2 Value page",
    IDM_LAYOUT_DETAILS_PAGE, "3 Details page",
    IDM_LAYOUT_INFO_PAGE,    "4 Info page",
};
const size_t menu_layout_size = SIZEOFARRAY(menu_layout); 

const MenuItem menu_display_off[] {
    IDM_TITLE,               "Display timeout",
    IDM_MAIN,                "1 Main menu",
    IDM_DISPLAY_OFF_NEVER,   "2 Never",
    IDM_DISPLAY_OFF_10SEC,   "3 After 10 sec.",
    IDM_DISPLAY_OFF_1MIN,    "3 After 1 min.",
    IDM_DISPLAY_OFF_5MIN,    "4 After 5 min.",
    IDM_DISPLAY_OFF_15MIN,   "5 After 15 min.",
    IDM_DISPLAY_OFF_30MIN,   "6 After 30 min.",
};
const size_t menu_display_off_size = SIZEOFARRAY(menu_display_off); 
static const float display_timeout_times[] = { 0.0f, 10.0f, 60.0f, 300.0f, 900.0f, 1800.0f };

const MenuItem menu_led[] {
    IDM_TITLE,              "LED intensity",
    IDM_MAIN,               "1 Main menu",
    IDM_LED_INTENSITY_100,  "2 Max. 100%",
    IDM_LED_INTENSITY_75,   "3 High 75%",
    IDM_LED_INTENSITY_50,   "4 Medium 50%",
    IDM_LED_INTENSITY_25,   "5 Low 25%",
    IDM_LED_INTENSITY_10,   "6 Very low 5%",
    IDM_LED_INTENSITY_1,    "7 Min. 1%",
};
const size_t menu_led_size = SIZEOFARRAY(menu_led); 
static const float LED_Intensity_List[] = { 1.0f, 0.75f, 0.5f, 0.25f, 0.05f, 0.01f, };

AppState App::print_Menu(void) {
    int menu_max = 0;
    if (m.display->has_DisplayOfType(DisplayType::OLED128x64)) {
        menu_max = 4;
    } else if (m.display->has_DisplayOfType(DisplayType::LCD16x2)) {
        menu_max = 2;
    }

    if ((m.menu != nullptr) && (m.menu_count > 1)) {
        m.display->clear();
        m.display->print(0, 0, m.menu[0].string, strlen(m.menu[0].string));
        int n = m.menuStart;
        for (int i = 1; (i < menu_max) && (n < m.menu_count); i++) {
            m.display->print(0, i, m.menu[n].string, strlen(m.menu[n].string));
            if (n == m.menuSelect) {
                m.display->invert_row(i);
            }
            n++;
        }
    }

    m.display->update();
    return (AppState::OK);
}

AppState App::handle_menu(void) {
    if ((m.button == nullptr) || (m.display == nullptr)) {
        return (AppState::not_implemented);
    }

    if (m.flags.b.bButtonEvent == 0) {
        return (AppState::idle);
    }
    m.flags.b.bButtonEvent = 0;    

    if (m.btnState.button_message == BTN_LONG_PRESS) {
        if (m.display_page == DisplayPage::menu) {
            if (exit_Menu()) {
                m.display_page = DisplayPage::value_page;
                m.display_request++;
                return (AppState::idle);
            }
        } else {
            m.display_page = DisplayPage::menu;
            set_Menu(menu_main, menu_main_size);
        }
    } 
    
    if (m.display_page != DisplayPage::menu) {
        m.display_request++;
        return (AppState::idle);
    }

    if (m.btnState.button_message == BTN_SHORT_PRESS) {
        step_Menu();
    }

    print_Menu();

    return (AppState::OK);
}

void App::set_Menu(const MenuItem* items, const size_t count) {
    if ((items == nullptr) || (count < 2)) {
        return;
    }
    reset_Menu();
    m.menu = items;
    m.menu_count = count;
}

void App::reset_Menu(void) {
    m.menuSelect = 1;
    m.menuStart = 1;

    if (m.display != nullptr) {
        m.display->clear();
        m.display->update();
    }
}

void App::step_Menu(void) {
    m.menuSelect++;
    if (m.menuSelect >= m.menu_count) {
        m.menuSelect = 1;
        m.menuStart = 1;
    } else {
        if ((m.menuSelect - m.menuStart) >= 3) {
            m.menuStart++;
        }
    }
}

size_t App::new_DynamicMenu(const char* menu_title) {
    m.dynamicMenu[0].set(IDM_TITLE, menu_title);
    m.dynamicMenu[1].set(IDM_EXIT, "1 exit");

    size_t size = sizeof(m.dynamicMenuString);
    memset(m.dynamicMenuString, 0, size);

    return (size);
}

AppState App::select_driver(void) {
    size_t size = new_DynamicMenu("Sensors");
    if (m.driver != nullptr) {
        size_t count = m.driver->search(APP_DRV_LIST_MAX, m.drv_scan_name, m.drv_scan_adr, m.drv_scan_type);
        if (count > 0) {
            int len = 0;
            for (size_t i = 0; (i < count) && (len < (size - 16)); i++) {
                char* string = &m.dynamicMenuString[len];
                snprintf(string, MIN(16, (size_t)size - len - 1), "%d %-7s (%-2.2X)", i + 2, m.drv_scan_name[i], (unsigned int)m.drv_scan_adr[i]);
                string[15] = 0;
                len += (strlen(string) + 1);
                m.dynamicMenu[i + 2].set((IDM_SENSOR_BASE + (int)i), string);
            }
            set_Menu(m.dynamicMenu, count + 2);
        }
    }

    return (AppState::OK);
}

AppState App::switch_driver_to(uint8_t i2c_addr) {
    if (m.sensor == nullptr) {
        return (AppState::not_ready);
    }
    m.driver = nullptr;
    m.sensor->set_driver(m.driver);
    m.flags.b.bDriverReady = 0;
    m.driver = SensorDriver::create_driver_by_address(i2c_addr);
    if (m.driver != nullptr) {
        m.cfg->set_sensor_type(m.driver->get_sensor_type());
        request_sys_config_update();
    }
    return (AppState::OK);
}

bool App::exit_Menu(void) {
    if ((m.menu[m.menuSelect].id >= IDM_SENSOR_BASE) && (m.menu[m.menuSelect].id < IDM_SENSOR_LAST)) {
        int i = m.menu[m.menuSelect].id - IDM_SENSOR_BASE;
        switch_driver_to(m.drv_scan_adr[i]);
        return (true);
    } else if ((m.menu[m.menuSelect].id >= IDM_LED_INTENSITY_BASE) && (m.menu[m.menuSelect].id <= IDM_LED_INTENSITY_LAST)) {
        int i = m.menu[m.menuSelect].id - IDM_LED_INTENSITY_BASE;
        m.cfg->set_LED_intensity(LED_Intensity_List[i]);
        request_sys_config_update();
        return (true);
    } else if ((m.menu[m.menuSelect].id >= IDM_DISPLAY_OFF_BASE) && (m.menu[m.menuSelect].id <= IDM_DISPLAY_OFF_LAST)) {
        int i = m.menu[m.menuSelect].id - IDM_DISPLAY_OFF_BASE;
        m.cfg->set_display_timeout(display_timeout_times[i]);
        request_sys_config_update();
        reload_display_timeout();
        m.display_request++;
        return (true);
    } else {
        switch (m.menu[m.menuSelect].id) {
            case IDM_EXIT: {
            } return (true);

            case IDM_MAIN: {
                set_Menu(menu_main, menu_main_size);
            } break;

            case IDM_DISPLAY_OFF: {
                set_Menu(menu_display_off, menu_display_off_size);
            } break;

            case IDM_LAYOUT: {
                set_Menu(menu_layout, menu_layout_size);
            } break;

            case IDM_DISPLAY: {
                set_Menu(menu_display, menu_display_size);
            } break;

            case IDM_ROTATE: {
                flip_display();
                request_sys_config_update();
            } return (true);

            case IDM_CONTRAST: {
                set_Menu(menu_contrast, menu_contrast_size);
            } break;

            case IDM_LED_INTENSITY: {
                set_Menu(menu_led, menu_led_size);
            } break;

            case IDM_SENSOR_SELECT: {
                select_driver();
            } break;

            case IDM_REBOOT: {
                esp_event_post(APP_EVENT, (int32_t)AppEvent::reboot, nullptr, 0, pdMS_TO_TICKS(1));
            } return (true);

            case IDM_FACTORY_RESET: {
                esp_event_post(APP_EVENT, (int32_t)AppEvent::factory_reset, nullptr, 0, pdMS_TO_TICKS(1));
            } return (true);

            case IDM_CONFIG: {
                set_Menu(menu_config, menu_config_size);
            } break;

            case IDM_CONFIG_INTERFACE: {
                handle_config_interface();
            } return (true);

            case IDM_LAYOUT_VALUE_PAGE: {
                m.cfg->set_display_layout(DisplayLayout::large_values);
                request_sys_config_update();
            } return (true);

            case IDM_LAYOUT_DETAILS_PAGE: {
                m.cfg->set_display_layout(DisplayLayout::detailes);
                request_sys_config_update();
            } return (true);

            case IDM_LAYOUT_INFO_PAGE: {
                m.cfg->set_display_layout(DisplayLayout::info);
                request_sys_config_update();
            } return (true);

            case IDM_CONTRAST_HIGH: {
                set_display_contrast(DISPLAY_CONTRAST_HIGH);
            } return (true);

            case IDM_CONTRAST_MEDIUM: {
                set_display_contrast(DISPLAY_CONTRAST_MEDIUM);
            } return (true);

            case IDM_CONTRAST_LOW: {
                set_display_contrast(DISPLAY_CONTRAST_LOW);
            } return (true);
            
            default: {
            } return (true);
        }
    }

    return (false);
}

AppState App::handle_config_interface(void) {
    if (m.cmd == nullptr) {
        m.cmd = new ConfigInterface(this);
        m.cfg->set_config_enable(CONFIG_IFC_ENABLED);
    } else {
        SAFE_DELETE(m.cmd);
        m.cfg->set_config_enable(CONFIG_IFC_DISABLED);
    }
    request_sys_config_update();

    if (m.display != nullptr) {
        m.display->clear();
        m.display->print(0, 1, "Config interface");
        if (m.cmd == nullptr) {
            m.display->print(0, 2, "is disabled.");
        } else {
            m.display->print(0, 2, "is enabled.");
        }
        m.display->update();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return (AppState::OK);
}
