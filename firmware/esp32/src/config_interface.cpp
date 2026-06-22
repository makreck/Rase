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

void ConfigInterface::init(void) {
    setup();
    xTaskCreate(ConfigInterface::_communication_handler, "ConfigInterface", TASK_DEFAULT_STACKSIZE, this, TASK_DEFAULT_PRIORITY, &task_handle);
}

void ConfigInterface::cleanup(void) {
	if (task_handle != nullptr) {
		vTaskDelete(task_handle);
		task_handle = nullptr;
	}
}

void ConfigInterface::setup(void) {
    // Avoid conflicts with JTAG interface during boot sequence!
    vTaskDelay(pdMS_TO_TICKS(1000));

    uart_config_t uart_config;
    uart_config.baud_rate  = 115200;
    uart_config.data_bits  = UART_DATA_8_BITS;
    uart_config.parity     = UART_PARITY_DISABLE;
    uart_config.stop_bits  = UART_STOP_BITS_1;
    uart_config.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_APB;
    uart_param_config(UART_NUM_0, &uart_config);

    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    usb_serial_jtag_driver_config_t jtag_cfg;
    jtag_cfg.tx_buffer_size = TX_BUFFER_SIZE;
    jtag_cfg.rx_buffer_size = RX_BUFFER_SIZE;
    usb_serial_jtag_driver_install(&jtag_cfg);
}

void ConfigInterface::_communication_handler(void *pvParameters) {
    (reinterpret_cast<ConfigInterface*>(pvParameters))->communication_handler();
}
void ConfigInterface::communication_handler(void) {
    while (true) {
        int bytes_read = usb_serial_jtag_read_bytes(rx_buffer, RX_BUFFER_SIZE, pdMS_TO_TICKS(100));
        if (bytes_read > 0) {
            process_command(rx_buffer, (size_t)bytes_read);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

ssize_t ConfigInterface::send(const char* data, size_t length) {
    if (data == nullptr) {
        return (0);
    }

    if (length == 0) {
        length = strlen(data);
    }

    size_t written = 0;
    while (written < length) {
        size_t to_write = (length - written > TX_BUFFER_SIZE) ? TX_BUFFER_SIZE : (length - written);
        ssize_t actual_written = (ssize_t)usb_serial_jtag_write_bytes(&data[written], to_write, pdMS_TO_TICKS(1000));
        if (actual_written <= 0) {
            break;
        }
        written += actual_written;
    }
    return (written);
}

void ConfigInterface::handle_wifi_setup(ConfigInterface* instance, int mode, const char* data, size_t length) {
    if (instance == nullptr) {
        return;
    }

    App* app = instance->app;

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, data, sizeof (buffer) - 1);
    buffer[length] = 0;

    char ssid[32];
    memset(ssid, 0, sizeof (ssid));
    char pwd[64];
    memset(pwd, 0, sizeof (pwd));

    int i = strlen(CFG_KEY_WIFI_SETUP);
    if (buffer[i] == '=') {
        char* p1 = &buffer[i + 1];
        char* p2 = strstr(p1, ":");
        if (p2 != nullptr) {
            *p2++ = 0;
            strncpy(pwd, p2, sizeof (pwd) - 1);
        }
        strncpy(ssid, p1, sizeof (ssid) - 1);
    }

    SysConfig* cfg = app->get_config();
    if (cfg != nullptr) {
        cfg->set_ssid(ssid);
        cfg->set_password(pwd);
#ifdef DISPLAY_STATE        
        cfg->print_parms("Config response");
#endif        
    }

    DisplayI2C* display = app->get_display();
    if (display != nullptr) {
        app->set_display_page(DisplayPage::menu);
        display->clear();
        display->print(0, 0, "WiFi-setup:");
        display->print(0, 1, ssid);
        display->print(0, 2, pwd);
        display->print(0, 3, "rebooting in 3s");
        display->update();
    }

    app->request_sys_config_update();
    vTaskDelay(pdMS_TO_TICKS(3000));
    esp_event_post(APP_EVENT, (int32_t)AppEvent::reboot, nullptr, 0, pdMS_TO_TICKS(1));
}

void ConfigInterface::handle_id_response(ConfigInterface* instance, int mode, const char* data, size_t length) {
    char *device_id_json = Tools::get_device_id_json();
    send(device_id_json, strlen(device_id_json));
    free(device_id_json);
}

void ConfigInterface::handle_sensor_response(ConfigInterface* instance, int mode, const char* data, size_t length) {
    size_t len = 0;
    char *json_response = instance->app->get_sensor()->get_json(len);
    send(json_response, len);
    free(json_response);
}

void ConfigInterface::handle_website_output(ConfigInterface* instance, int mode, const char* data, size_t length) {
    size_t len = strlen(WEB_SITE_BASE_STRING);
    send(WEB_SITE_BASE_STRING, len);
}

void ConfigInterface::handle_restart(ConfigInterface* instance, int mode, const char* data, size_t length) {
    AppEvent message = (mode == 9) ? AppEvent::factory_reset : AppEvent::reboot;
    esp_event_post(APP_EVENT, (int32_t)message, nullptr, 0, pdMS_TO_TICKS(1));
}

void ConfigInterface::handle_config_response(ConfigInterface* instance, int mode, const char* data, size_t length) {
// @TODO: Reveive device configuration JSON string.
    const char* msg = "Still under construction!\n";
    send(msg, strlen(msg));
}

void ConfigInterface::process_command(const char* data, size_t length) {
    for (size_t i = 0; i < SIZEOFARRAY(function_tab); i++) {
        if (strncmp(data, function_tab[i].key, strlen(function_tab[i].key)) == 0) {
            (*function_tab[i].callback)(this, function_tab[i].mode, data, length);
            break;
        }
    }
}
