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

AppState App::init(void) {
    init_watchdog();
    init_event_loop();
    init_config();
    init_LEDs();
    init_display();
    init_buttons();
    init_wifi();
    init_driver();
    init_webserver();    

    return (AppState::OK);
}

AppState App::cleanup(void) {
    m.exitApp = true;

    if (m.task_handle != nullptr) {
        vTaskDelete(m.task_handle);
        m.task_handle = nullptr;
    }

    SAFE_DELETE(m.webserver);
    SAFE_DELETE(m.driver);
    SAFE_DELETE(m.sensor);
    SAFE_DELETE(m.station);
    SAFE_DELETE(m.display);
    SAFE_DELETE(m.button);
    SAFE_DELETE(m.led);
    SAFE_DELETE(m.cfg);

    esp_event_handler_unregister(APP_EVENT, ESP_EVENT_ANY_ID, &App::_app_event_handler);
    esp_task_wdt_deinit();

    return (AppState::OK);
}

AppState App::init_watchdog(uint32_t timeout_ms) {
    esp_task_wdt_config_t wdt = {
        .timeout_ms     = timeout_ms,
        .idle_core_mask = 0x03,
        .trigger_panic  = true,
    };
    esp_task_wdt_deinit();
    esp_task_wdt_init(&wdt);
    esp_task_wdt_add(nullptr);
    return (AppState::OK);
}

AppState App::init_config(void) {
    m.cfg = new SysConfig();
    if ((m.cfg->get_config_enable() & CONFIG_IFC_ENABLED) == CONFIG_IFC_ENABLED) {
        m.cmd = new ConfigInterface(this);
    }
    return ((m.cfg != nullptr) ? AppState::OK : AppState::failed);
}

AppState App::init_event_loop(void) {
    esp_event_loop_create_default();
    esp_err_t result = esp_event_handler_register(APP_EVENT, ESP_EVENT_ANY_ID, &App::_app_event_handler, this);
    AppState state = ((result == ESP_OK) ? AppState::OK : AppState::failed);
    return (state);
}

AppState App::init_buttons(void) {
#ifdef _ENABLE_BUTTON    
    m.button = new Button();
    return ((m.button != nullptr) ? AppState::OK : AppState::failed);
#else
    return (AppState::not_implemented);
#endif    
}

AppState App::init_wifi(void) {
    esp_netif_init();
    
    const char* ssid = m.cfg->get_ssid();
    const char* pwd = m.cfg->get_password();
    if (strlen(ssid) > 0) {
        m.station = new Wifi_Station(ssid, pwd);
    }
    
    AppState state = ((m.station != nullptr) ? AppState::OK : AppState::failed);
    return (state);
}

AppState App::init_webserver(void) {
    m.webserver = new WebServer();
    AppState state = ((m.webserver != nullptr) ? AppState::OK : AppState::failed);
    return (state);
}

AppState App::init_driver(void) {
    m.sensor = new SensorDevice("ESP32-S3");
    m.driver = SensorDriver::auto_scan(m.cfg->get_sensor_type());

    //m.driver = new SensorSHT2(); // ****
    //m.driver = SensorDriver::auto_scan(SensorType::SHT2x); // ****
    //m.driver = SensorDriver::auto_scan(SensorType::SHT3x); // ****
    //m.driver = SensorDriver::auto_scan(SensorType::HTU21d); // ****
    //m.driver = SensorDriver::auto_scan(SensorType::BMx280); // ****

    return (((m.sensor != nullptr) && (m.driver != nullptr)) ? AppState::OK : AppState::failed);
}

AppState App::trigger_watchdog(void) {
    if (m.exitApp) {
        return AppState::exit;
    }
    return ((esp_task_wdt_reset() == ESP_OK) ? AppState::OK : AppState::watchdog);
}

void App::_app_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    (reinterpret_cast<App*>(arg))->app_event_handler(event_base, (AppEvent)event_id, event_data);
}
esp_err_t App::app_event_handler(esp_event_base_t event_base, AppEvent event_id, void* event_data) {
    switch (event_id) {
        case AppEvent::measuring_event: {
            if (m.sensor != nullptr) {
                m.sensor->update((SensorReading*)event_data);
                m.flags.b.bDriverQuery = 1;
                m.display_request++;
            }
        } break;

        case AppEvent::web_query_event: {
            m.flags.b.bWebsiteQuery = 1;
        } break;

        case AppEvent::web_favicon_req: {
        } break;

        case AppEvent::web_api_event: {
            m.flags.b.bWebAPIQuery = 1;
        } break;

        case AppEvent::button_event: {
            m.btnState.set((ButtonState*)event_data);
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Button event: msg=%d, multi=%d, ms_pressed=%d, ms_released=%d, ms_idle=%d", 
                m.btnState.button_message, m.btnState.press_count, (int)m.btnState.time_pressed_ms, (int)m.btnState.time_released_ms, (int)m.btnState.time_idle_ms);
#endif
        } break;

        case AppEvent::button_idle: {
            m.flags.b.bButtonEvent = 1;
        } break;

        case AppEvent::button_ready: {
            m.flags.b.bButtonReady = 1;
        } break;

        case AppEvent::display_ready: {
            m.flags.b.bDisplayReady = 1;
        } break;

        case AppEvent::wifi_enabled: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Wifi is enabled.");
#endif
            m.flags.b.bWifiEnabled = 1;
            m.display_request++;
        } break;

        case AppEvent::wifi_AP_not_found: {
#ifdef DISPLAY_STATE
            ESP_LOGE(TAG, "Error, AP \"%s\" not found!", m.cfg->get_ssid());
#endif
        } break;

        case AppEvent::wifi_disconnected: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Disconnected from Wifi AP \"%s\".", m.cfg->get_ssid());
#endif
        } break;

        case AppEvent::wifi_connected: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Connected to Wifi AP \"%s\".", m.cfg->get_ssid());
#endif
            m.flags.b.bWifiConnected = 1;
            m.display_request++;
        } break;

        case AppEvent::driver_ready: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Sensor driver is ready.");
#endif
            m.flags.b.bDriverReady = 1;

            m.display_page = m.cfg->get_display_layout();
            m.display_request++;

            if (m.sensor != nullptr) {
                m.sensor->set_driver(m.driver);
            } else {
                ESP_LOGE(TAG, "App.event: Driver ready. Fatal error, try to activate driver with no Sensor object!");
            }

            vTaskDelay(pdMS_TO_TICKS(500));
            esp_event_post(APP_EVENT, (int32_t)AppEvent::web_start_server, nullptr, 0, pdMS_TO_TICKS(1));
        } break;

        case AppEvent::web_start_server: {
            if ((m.flags.b.bWebsiteReady  == 0) && 
                (m.flags.b.bWifiConnected == 1) && 
                (m.flags.b.bDriverReady   == 1)) {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Webserver is starting...");
#endif
                m.webserver->start(m.sensor);
            } else {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Webserver start is delayed because the sensor driver is not ready.");
#endif
                vTaskDelay(pdMS_TO_TICKS(1000));
                esp_event_post(APP_EVENT, (int32_t)AppEvent::web_start_server, nullptr, 0, pdMS_TO_TICKS(1));
            }
        } break;

        case AppEvent::web_started: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Webserver is running.");
#endif
            m.flags.b.bWebsiteReady = 1;
            m.display_request++;
        } break;

        default: {
        } break;

    }

    return (ESP_OK);
}

AppState App::handle_nvm_update(void) {
    if (m.flags.b.bNVMUpdateReq == 1) {
        m.flags.b.bNVMUpdateReq = 0;
        m.cfg->update();
    }
    return (AppState::idle);
}

AppState App::handle_reset(bool init_flash) {
    if (m.display != nullptr) {
        m.display->clear();
        m.display->print(0, 0, (init_flash) ? "Factory reset..." : "System restart..");
        m.display->update();
    }

    if (init_flash) {
        m.cfg->perform_factory_reset();
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
    cleanup();
    esp_restart();
    
    return (AppState::OK);
}

AppState App::run(void) {
    while (trigger_watchdog() == AppState::OK) {
        handle_nvm_update();
        handle_LEDs();
        handle_display();
        handle_menu();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
    return (AppState::OK);
}
