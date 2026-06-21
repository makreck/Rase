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

#include "includes.hpp"

#include "version.hpp"
#include "typedef.hpp"
#include "app_config.hpp"
#include "io_mapping.hpp"

#include "app_state.hpp"
#include "tools.hpp"
#include "events.hpp"
#include "color.hpp"
#include "sw_i2c_hal.hpp"
#include "display_i2c.hpp"
#include "button.hpp"
#include "led_controller.hpp"
#include "wifi.hpp"
#include "config_interface.hpp"
#include "driver/sensor_address.hpp"
#include "driver/sensor_driver.hpp"
#include "driver/sensor_null.hpp"
#include "driver/sensor_bm2.hpp"
#include "driver/sensor_htu21d.hpp"
#include "driver/sensor_aht21.hpp"
#include "driver/sensor_sht2.hpp"
#include "driver/sensor_sht3.hpp"
#include "driver/sensor_hdc1080.hpp"
#include "channel_history.hpp"
#include "sensor_node.hpp"
#include "sensor_device.hpp"
#include "web_server.hpp"
#include "sys_config.hpp"
#include "config_interface.hpp"
#include "app_menu.hpp"

#define WEB_SITE_BASE_STRING    WebServer::webserver_resp_str_2

#define TASK_DEFAULT_PRIORITY   (20)
#define TASK_SMALL_STACKSIZE    (1024)
#define TASK_DEFAULT_STACKSIZE  (4096)
#define TASK_EXTENDED_STACKSIZE (8192)

#define APP_DRV_LIST_MAX (16)

class App {
    private:

        struct {
            bool             exitApp          = false;
            TaskHandle_t     task_handle       = nullptr;
            
            SysConfig*       cfg              = nullptr;
            LEDController*   led              = nullptr;
            Button*          button           = nullptr;
            DisplayI2C*      display          = nullptr;
            Wifi_Station*    station          = nullptr;
            WebServer*       webserver        = nullptr;
            SensorDriver*    driver           = nullptr;
            SensorDevice*    sensor           = nullptr;
            ConfigInterface* cmd              = nullptr;
            
            SensorReading    reading;
            ButtonState      btnState;

            const MenuItem*  menu             = nullptr;
            size_t           menu_count       = 0;
            int32_t          menuSelect       = 1;
            int32_t          menuStart        = 1;
            MenuItem         dynamicMenu[16]{ 0, nullptr };
            char             dynamicMenuString[256]{ 0 };

            int32_t          display_request  = 0;
            int32_t          display_update   = 0;
            DisplayPage      display_page     = DisplayPage::invalid;
            uint32_t         display_off_time = 0;

            union {
                uint32_t f = 0;
                struct {
                    uint32_t bError          : 1;
                    uint32_t bNVMUpdateReq   : 1;
                    uint32_t bDisplayOFF     : 1;

                    uint32_t bButtonReady    : 1;
                    uint32_t bDisplayReady   : 1;
                    uint32_t bDriverReady    : 1;
                    uint32_t bWifiEnabled    : 1;
                    uint32_t bWifiConnected  : 1;
                    uint32_t bWebsiteReady   : 1;

                    uint32_t bButtonEvent    : 1;
                    uint32_t bDriverQuery    : 1;
                    uint32_t bWebAPIQuery    : 1;
                    uint32_t bWebsiteQuery   : 1;
                } b;
            } flags;

            const char*      drv_scan_name[APP_DRV_LIST_MAX]{ nullptr };
            uint8_t          drv_scan_adr[APP_DRV_LIST_MAX]{0};
            SensorType       drv_scan_type[APP_DRV_LIST_MAX]{ SensorType::invalid };
        } m;

        AppState init_watchdog(uint32_t timeout_ms = 60000);
        AppState init_config(void);
        AppState init_event_loop(void);
        AppState init_display(void);
        AppState init_LEDs(void);
        AppState init_buttons(void);
        AppState init_wifi(void);
        AppState init_webserver(void);
        AppState init_driver(void);

        AppState print_net_Info(void);
        AppState print_measuring_info(uint8_t mode = 0);
        AppState print_large_text(int start);
        AppState print_Menu(void);

        void     display_next(void);
        void     flip_display(void);
        bool     reload_display_timeout(void);
        void     set_display_contrast(float value);
        void     set_Menu(const MenuItem* items, const size_t count);
        void     reset_Menu(void);
        bool     exit_Menu(void);
        void     step_Menu(void);
        size_t   new_DynamicMenu(const char* menu_title);

        AppState select_driver(void);
        AppState switch_driver_to(uint8_t i2c_addr);
        AppState trigger_watchdog(void);

        AppState handle_nvm_update(void);
        AppState handle_LEDs(void);
        AppState handle_menu(void);
        AppState handle_display(void);
        AppState handle_config_interface(void);

        AppState reload_screensaver(void);
        AppState check_screensaver(void);

        AppState display_value_page(void);
        AppState display_details_page(void);
        AppState display_info_page(void);

        AppState display_small_value_page(void);

        static void _app_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        esp_err_t app_event_handler(esp_event_base_t event_base, AppEvent event_id, void* event_data);

    public:
        App() {
            init();
        }
        
        ~App() {
            cleanup();
        }

        AppState init(void);
        AppState run(void);
        AppState cleanup(void);
        AppState request_sys_config_update(void);
        AppState handle_reset(bool init_flash = false);
        AppState set_display_page(DisplayPage page);

        SysConfig*       get_config(void)    { return (m.cfg);       }
        LEDController*   get_LED(void)       { return (m.led);       }
        Button*          get_button(void)    { return (m.button);    }
        DisplayI2C*      get_display(void)   { return (m.display);   }
        Wifi_Station*    get_station(void)   { return (m.station);   }
        WebServer*       get_webserver(void) { return (m.webserver); }
        SensorDriver*    get_driver(void)    { return (m.driver);    }
        SensorDevice*    get_sensor(void)    { return (m.sensor);    }
        ConfigInterface* get_cmd(void)       { return (m.cmd);       }
};
