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

#define HTTP_MAX_ENDPOINTS  (10)
#define HTTP_DEFAULT_PORT   (80)
#define HTTP_CONTROL_PORT   (32768)
#define HTTP_MAX_HEADERS    (16)
#define HTTP_MAX_SOCKETS    (7)

const WebServerURI WebServer::web_uri_tab[HTTP_MAX_ENDPOINTS] = {
    { WEB_KEY_ROOT,         HTTP_GET,  WebServer::_root_handler    },
    { WEB_KEY_CONFIG_ROOT,  HTTP_GET,  WebServer::_root_handler    },
    { WEB_KEY_UPDATE_ROOT,  HTTP_GET,  WebServer::_root_handler    },

    { WEB_KEY_API_SENSORS,  HTTP_GET,  WebServer::_api_handler     },
    { WEB_KEY_API_IDENTIFY, HTTP_GET,  WebServer::_api_handler     },
    { WEB_KEY_API_CONFIG,   HTTP_GET,  WebServer::_api_handler     },
    { WEB_KEY_API_CONFIG,   HTTP_PUT,  WebServer::_api_handler     },
    { WEB_KEY_API_UPDATE,   HTTP_PUT,  WebServer::_api_handler     },
    { WEB_KEY_API_REBOOT,   HTTP_GET,  WebServer::_api_handler     },

    { WEB_KEY_FAVICON,      HTTP_GET,  WebServer::_favicon_handler },
};

const char* WebServer::favicon_svg =    "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" "
                                        "stroke-linecap=\"round\" stroke-linejoin=\"round\" width = \"20\" height = \"20\" > "
                                        "<path d=\"M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z\"></path></svg>";


esp_err_t WebServer::init(void) {
    m.config.stack_size       = TASK_EXTENDED_STACKSIZE;
    m.config.server_port      = HTTP_DEFAULT_PORT;
    m.config.ctrl_port        = HTTP_CONTROL_PORT;
    m.config.max_uri_handlers = HTTP_MAX_ENDPOINTS;
    m.config.max_resp_headers = HTTP_MAX_HEADERS;
    m.config.max_open_sockets = HTTP_MAX_SOCKETS;
    m.config.lru_purge_enable = true;

    return (ESP_OK);
}

esp_err_t WebServer::start(App* _app, const char* _ip_addr) {
    m.sensor  = _app->get_sensor();
    m.display = _app->get_display();
    m.cfg     = _app->get_config();
    
    if (_ip_addr != nullptr) {
        strncpy(m.ip_addr, _ip_addr, sizeof (m.ip_addr));
    }

    esp_err_t err = httpd_start(&m.server, &m.config);
    if (err != ESP_OK) {
#ifdef DISPLAY_STATE        
        ESP_LOGE(TAG, "Failed to start HTTP server: %s", esp_err_to_name(err));
#endif
        return (err);
    }

    for (size_t i = 0; i < SIZEOFARRAY(WebServer::web_uri_tab); i++) {
        httpd_uri_t web_uri = {
            .uri       = WebServer::web_uri_tab[i].http_uri,
            .method    = WebServer::web_uri_tab[i].method,
            .handler   = WebServer::web_uri_tab[i].handler,
            .user_ctx  = this,
        };
        err = httpd_register_uri_handler(m.server, &web_uri);
        if (err != ESP_OK) break;
    }
    
#ifdef DISPLAY_STATE        
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register HTTP handler: %s", esp_err_to_name(err));
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

void WebServer::_time_sync_notification(struct timeval* tv) {
    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_time_sync, nullptr, 0, pdMS_TO_TICKS(1));
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

    esp_sntp_set_time_sync_notification_cb(_time_sync_notification);

    esp_sntp_init();

    return (ESP_OK);
}


esp_err_t WebServer::_favicon_handler(httpd_req_t* req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->favicon_handler(req));
}
esp_err_t WebServer::favicon_handler(httpd_req_t* req) {

#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::favicon_handler() event.");
#endif

    httpd_resp_set_type(req, "image/svg+xml");
    httpd_resp_set_hdr(req, "Cache-Control", "max-age=60");
    httpd_resp_send(req, favicon_svg, HTTPD_RESP_USE_STRLEN);

    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_query_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}
