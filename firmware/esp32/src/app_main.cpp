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

#define DISPLAY_STATE

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
    init_mqtt();

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
    if (m.cfg->get_config_enable()) {
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
    return (((m.station != nullptr) ? AppState::OK : AppState::failed));
}

AppState App::init_webserver(void) {
    m.webserver = new WebServer();
    return ((m.webserver != nullptr) ? AppState::OK : AppState::failed);
}

AppState App::init_mqtt(void) {
    if (m.cfg->get_mqtt_enable()) {
        const char* broker = m.cfg->get_mqtt_broker();
        if (strlen(broker) > 0) {
            m.mqtt = new Mqtt(m.cfg->get_mqtt_broker(), m.cfg->get_mqtt_username(), m.cfg->get_mqtt_password());
        }
    }
    return ((m.mqtt != nullptr) ? AppState::OK : AppState::failed);
}

AppState App::init_driver(void) {
    m.sensor = new SensorDevice("ESP32-S3");
    m.driver = SensorDriver::auto_scan(m.cfg->get_sensor_type());
    return (((m.sensor != nullptr) && (m.driver != nullptr)) ? AppState::OK : AppState::failed);
}

AppState App::trigger_watchdog(void) {
    if (m.exitApp) {
        return AppState::exit;
    }
    return ((esp_task_wdt_reset() == ESP_OK) ? AppState::OK : AppState::watchdog);
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

AppState App::switch_driver_to(uint8_t i2c_addr) {
ESP_LOGI(TAG, "switch_driver_to(%d) - 1", (int)i2c_addr);
    if ((m.driver == nullptr) || (m.sensor == nullptr)) {
        return (AppState::not_ready);
    }

    if (m.driver->get_device_address() == i2c_addr) {
ESP_LOGI(TAG, "switch_driver_to(%d) - 2 - nothing to do.", (int)i2c_addr);
        return (AppState::OK);
    }

ESP_LOGI(TAG, "switch_driver_to(%d) - 3 - Sensor manager driver disable.", (int)i2c_addr);
    m.sensor->set_driver(nullptr);

    if (m.mqtt != nullptr) {
ESP_LOGI(TAG, "switch_driver_to(%d) - 4 - MQTT stop, driver disable.", (int)i2c_addr);
        m.mqtt->stop();
    }

ESP_LOGI(TAG, "switch_driver_to(%d) - 5 - Suspend all driver operations.", (int)i2c_addr);
    m.driver->suspend();
    m.flags.b.driver_ready = 0;
    
ESP_LOGI(TAG, "switch_driver_to(%d) - 6 - Delete the sensor driver.", (int)i2c_addr);
    SAFE_DELETE(m.driver);

ESP_LOGI(TAG, "switch_driver_to(%d) - 7 - Create a new sensor driver.", (int)i2c_addr);
    m.driver = SensorDriver::create_driver_by_address(i2c_addr);
    if (m.driver != nullptr) {
ESP_LOGI(TAG, "switch_driver_to(%d) - 9 - Set driver for Sensor manager.", (int)i2c_addr);
        m.sensor->set_driver(m.driver);

        if ((m.mqtt != nullptr) && (m.cfg->get_mqtt_enable() == true)) {
ESP_LOGI(TAG, "switch_driver_to(%d) - 10 - Set driver for MQTT and restart MQTT operation.", (int)i2c_addr);
            m.mqtt->start(m.driver);
        }
    }

ESP_LOGI(TAG, "switch_driver_to(%d) - 99", (int)i2c_addr);
    return (AppState::OK);
}

AppState App::request_sys_config_update(void) {
    m.update_thr_time = Tools::get_tickcount64() + 100;
    m.flags.b.nvm_update_req = 1;
    return (AppState::OK);
}

void App::_app_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    (reinterpret_cast<App*>(arg))->app_event_handler(event_base, (AppEvent)event_id, event_data);
}
esp_err_t App::app_event_handler(esp_event_base_t event_base, AppEvent event_id, void* event_data) {
    switch (event_id) {
        case AppEvent::measuring_event: {
            if (m.sensor != nullptr) {
                m.sensor->update((SensorReading*)event_data);
                m.flags.b.driver_query = 1;
                m.display_request++;
            }
        } break;

        case AppEvent::web_query_event: {
            m.flags.b.website_query = 1;
        } break;

        case AppEvent::web_favicon_req: {
        } break;

        case AppEvent::web_api_event: {
            m.flags.b.web_api_query = 1;
        } break;

        case AppEvent::button_event: {
            m.btnState.set((ButtonState*)event_data);
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Button event: msg=%d, multi=%d, ms_pressed=%d, ms_released=%d, ms_idle=%d", 
                m.btnState.button_message, m.btnState.press_count, (int)m.btnState.time_pressed_ms, (int)m.btnState.time_released_ms, (int)m.btnState.time_idle_ms);
#endif
        } break;

        case AppEvent::button_idle: {
            m.flags.b.button_event = 1;
        } break;

        case AppEvent::button_ready: {
            m.flags.b.button_ready = 1;
        } break;

        case AppEvent::display_ready: {
            m.flags.b.display_ready = 1;
        } break;

        case AppEvent::wifi_enabled: {
            m.flags.b.wifi_enabled = 1;
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
            m.flags.b.wifi_connected = 1;
            m.display_request++;

            esp_event_post(APP_EVENT, (int32_t)AppEvent::mqtt_configure, nullptr, 0, pdMS_TO_TICKS(1));
        } break;

        case AppEvent::driver_ready: {
            m.flags.b.driver_ready = 1;
            m.display_request++;

            if (m.sensor != nullptr) {
                m.sensor->set_driver(m.driver);
            } else {
                ESP_LOGE(TAG, "App.event: Driver ready. Fatal error, try to activate driver with no Sensor object!");
            }

            vTaskDelay(pdMS_TO_TICKS(500));
            esp_event_post(APP_EVENT, (int32_t)AppEvent::mqtt_configure, nullptr, 0, pdMS_TO_TICKS(1));
            esp_event_post(APP_EVENT, (int32_t)AppEvent::web_start_server, nullptr, 0, pdMS_TO_TICKS(1));
        } break;

        case AppEvent::web_start_server: {
            if ((m.flags.b.website_ready  == 0) && 
                (m.flags.b.wifi_connected == 1) && 
                (m.flags.b.driver_ready   == 1)) {
                m.webserver->start(m.sensor, m.station->get_ip());
            } else {
                vTaskDelay(pdMS_TO_TICKS(1000));
                esp_event_post(APP_EVENT, (int32_t)AppEvent::web_start_server, nullptr, 0, pdMS_TO_TICKS(1));
            }
        } break;

        case AppEvent::web_started: {
            m.flags.b.website_ready = 1;
            m.display_request++;
        } break;

        case AppEvent::mqtt_configure: {
            if ((m.flags.b.wifi_connected == 0) || (m.flags.b.driver_ready == 0)) {
                SAFE_DELETE(m.mqtt);
                break;
            }

            if (m.cfg->get_mqtt_enable()) {
                if (m.mqtt == nullptr) {
                    const char* broker = m.cfg->get_mqtt_broker();
                    if (strlen(broker) > 0) {
                        m.mqtt = new Mqtt(m.cfg->get_mqtt_broker(), m.cfg->get_mqtt_username(), m.cfg->get_mqtt_password());
                        m.mqtt->start(m.driver);
                    }
                }
            } else {
                if (m.mqtt != nullptr) {
                    SAFE_DELETE(m.mqtt);
                }
            }
        } break;

        case AppEvent::web_time_sync: {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "Webserver time synchronization event.");
#endif
            reload_screensaver();
        } break;

        case AppEvent::nvm_update: {
            request_sys_config_update();
        } break;

        case AppEvent::factory_reset: {
            handle_reset(true);
        } break;

        case AppEvent::reboot: {
            handle_reset(false);
        } break;

        case AppEvent::display_config: {
            m.flags.b.display_cfg_req = 1;
        } break;

        case AppEvent::driver_config: {
            m.flags.b.driver_cfg_req = 1;
        } break;
        
        default: {
#ifdef DISPLAY_STATE
            ESP_LOGE(TAG, "Unsupported message %d.", (int)event_id);
#endif
        } break;

    }

    return (ESP_OK);
}

AppState App::handle_config_changes(void) {
    if (m.flags.b.display_cfg_req == 1) {
        m.flags.b.display_cfg_req = 0;
        if (m.display != nullptr) {
            m.display->set_contrast(m.cfg->get_display_contrast());
            m.display->set_rotation(m.cfg->get_display_rotation());
            request_sys_config_update();
        }
    }

    if (m.flags.b.driver_cfg_req == 1) {
        m.flags.b.driver_cfg_req = 0;
        uint8_t i2c_addr = SensorDriver::get_bus_addr_by_type(m.cfg->get_sensor_type());
        switch_driver_to(i2c_addr);
        request_sys_config_update();
    }

    return (AppState::OK);
}

AppState App::handle_nvm_update(void) {
    if (m.flags.b.nvm_update_req == 1) {

        if (m.update_thr_time != 0) {
            if (Tools::get_tickcount64() < m.update_thr_time) {
#ifdef DISPLAY_STATE
            ESP_LOGI(TAG, "handle_nvm_update() -> delayed...");
#endif
                return (AppState::not_ready);
            }
            m.update_thr_time = 0;
        }

#ifdef DISPLAY_STATE
        ESP_LOGI(TAG, "handle_nvm_update() -> saving data...");
#endif
        m.flags.b.nvm_update_req = 0;
        m.cfg->update();
        vTaskDelay(pdMS_TO_TICKS(250));
#ifdef DISPLAY_STATE
        ESP_LOGI(TAG, "handle_nvm_update() -> Update finished.");
#endif
    }
    return (AppState::idle);
}

AppState App::run(void) {
    while (trigger_watchdog() == AppState::OK) {
        handle_config_changes();
        handle_nvm_update();
        handle_LEDs();
        handle_display();
        handle_menu();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
    return (AppState::OK);
}
