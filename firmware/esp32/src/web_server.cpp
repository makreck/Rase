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

// #define DISPLAY_STATE

static const char* favicon_svg =    "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" "
                                    "stroke-linecap=\"round\" stroke-linejoin=\"round\" width = \"20\" height = \"20\" > "
                                    "<path d=\"M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z\"></path></svg>";

esp_err_t WebServer::init(void) {
    m.config.stack_size       = TASK_EXTENDED_STACKSIZE;
    m.config.server_port      = 80;
    m.config.ctrl_port        = 32768;
    m.config.max_uri_handlers = 8;
    m.config.max_resp_headers = 16;
    m.config.max_open_sockets = 7;
    m.config.lru_purge_enable = true;

    return (ESP_OK);
}

esp_err_t WebServer::start(SensorDevice* _sensor) {
    m.sensor = _sensor;

    esp_err_t err = httpd_start(&m.server, &m.config);
    if (err != ESP_OK) {
#ifdef DISPLAY_STATE        
        ESP_LOGE(TAG, "Failed to start HTTP server: %s", esp_err_to_name(err));
#endif
        return (err);
    }

    do {
        httpd_uri_t root_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = WebServer::_root_handler,
            .user_ctx  = this,
        };
        err = httpd_register_uri_handler(m.server, &root_uri);
        if (err != ESP_OK) break;

        httpd_uri_t favicon_uri = {
            .uri       = "/favicon.ico",
            .method    = HTTP_GET,
            .handler   = WebServer::_api_favicon_handler,
            .user_ctx  = this,
        };
        err = httpd_register_uri_handler(m.server, &favicon_uri);
        if (err != ESP_OK) break;

        httpd_uri_t sensors_uri = {
            .uri       = "/api/sensors",
            .method    = HTTP_GET,
            .handler   = WebServer::_api_sensors_handler,
            .user_ctx  = this,
        };
        err = httpd_register_uri_handler(m.server, &sensors_uri);
        if (err != ESP_OK) break;

        httpd_uri_t opcua_uri = {
            .uri       = "/opcua/nodes",
            .method    = HTTP_GET,
            .handler   = WebServer::_opcua_get_handler,
            .user_ctx  = this,
        };
        err = httpd_register_uri_handler(m.server, &opcua_uri);
        if (err != ESP_OK) break;

        httpd_uri_t id_uri = {
            .uri       = "/api/id",
            .method    = HTTP_GET,
            .handler   = WebServer::_api_id_handler,
            .user_ctx  = this,
        };
        err = httpd_register_uri_handler(m.server, &id_uri);
        if (err != ESP_OK) break;

    } while(false);

#ifdef DISPLAY_STATE        
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server: %s", esp_err_to_name(err));
    }
#endif

    init_time_server();

    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_started, &m.config.server_port, sizeof (m.config.server_port), pdMS_TO_TICKS(10));
    return (err);
}

esp_err_t WebServer::cleanup(void) {
    return (stop());
}

esp_err_t WebServer::stop(void) {
    if (m.server) {
        httpd_stop(m.server);
        m.server = nullptr;
        esp_event_post(APP_EVENT, (int32_t)AppEvent::web_shutdown, nullptr, 0, pdMS_TO_TICKS(1));
    }
    return (ESP_OK);
}

esp_err_t WebServer::init_time_server(void) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::init_time_server().");
#endif
    setenv("TZ", TIME_ZONE_DEFAULT, 1);
    tzset();

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, CONFIG_SNTP_SERVER_0);
    esp_sntp_setservername(1, CONFIG_SNTP_SERVER_1);
    esp_sntp_setservername(2, CONFIG_SNTP_SERVER_2);
    esp_sntp_init();

    return (ESP_OK);
}

esp_err_t WebServer::_root_handler(httpd_req_t *req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->root_handler(req));
}
esp_err_t WebServer::root_handler(httpd_req_t *req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::root_handler() event.");
#endif
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, webserver_resp_str, HTTPD_RESP_USE_STRLEN);

    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_query_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}

esp_err_t WebServer::_api_favicon_handler(httpd_req_t *req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->api_favicon_handler(req));
}
esp_err_t WebServer::api_favicon_handler(httpd_req_t *req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::api_favicon_handler() event.");
#endif
    httpd_resp_set_type(req, "image/svg+xml");
    httpd_resp_set_hdr(req, "Cache-Control", "max-age=3600");
    httpd_resp_send(req, favicon_svg, HTTPD_RESP_USE_STRLEN);

    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_favicon_req, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}

esp_err_t WebServer::_api_sensors_handler(httpd_req_t *req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->api_sensors_handler(req));
}
esp_err_t WebServer::api_sensors_handler(httpd_req_t *req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::api_sensors_handler() event.");
#endif
    char time_string[TIME_STAMP_LENGTH]{0};
    Tools::get_timestamp(time_string, sizeof (time_string));

    if (m.sensor != nullptr) {
        size_t length = 0;
        char* json_response = m.sensor->get_json(length); 
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json_response, length);
        free(json_response);
    }

    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_api_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}

esp_err_t WebServer::_api_id_handler(httpd_req_t *req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->api_id_handler(req));
}
esp_err_t WebServer::api_id_handler(httpd_req_t *req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::api_id_handler() event.");
#endif
    char* device_id_json = Tools::get_device_id_json();
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, device_id_json, HTTPD_RESP_USE_STRLEN);
    free(device_id_json);
    
    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_api_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}

esp_err_t WebServer::_opcua_get_handler(httpd_req_t *req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->opcua_get_handler(req));
}
esp_err_t WebServer::opcua_get_handler(httpd_req_t *req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::api_opcua_get_handler() event.");
#endif
    size_t length = 0;
    char* opcua_json_response = m.sensor->get_opcua_json(length); 
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, opcua_json_response, length);
    free(opcua_json_response);
    
    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_api_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}
