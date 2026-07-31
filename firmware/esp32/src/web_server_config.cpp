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


esp_err_t WebServer::_config_root_handler(httpd_req_t *req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->config_root_handler(req));
}
esp_err_t WebServer::config_root_handler(httpd_req_t *req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::config_root_handler() event.");
#endif
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, WEB_SITE_BASE_STRING, HTTPD_RESP_USE_STRLEN);

    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_query_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}


esp_err_t WebServer::_api_cfg_get_handler(httpd_req_t *req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->api_cfg_get_handler(req));
}
esp_err_t WebServer::api_cfg_get_handler(httpd_req_t *req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::api_cfg_get_handler() event.");
#endif

    char* config_json = m.cfg->get_json(true); 
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, config_json, HTTPD_RESP_USE_STRLEN);
    free(config_json);
    
    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_api_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}


esp_err_t WebServer::_api_cfg_put_handler(httpd_req_t* req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->api_cfg_put_handler(req));
}
esp_err_t WebServer::api_cfg_put_handler(httpd_req_t* req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::api_cfg_put_handler() event. config write request, content length = %.1f KB.", (float)req->content_len / 1024.0f);
#endif

    if (req->content_len == 0) {
        httpd_resp_send_err(req, HTTPD_411_LENGTH_REQUIRED, nullptr);
        return (ESP_FAIL);
    }

    char* buffer = (char *)malloc(req->content_len + 8);
    if (buffer != nullptr) {
        memset(buffer, 0, req->content_len + 8);
        int read_bytes = httpd_req_recv(req, (char*)buffer, req->content_len);
        if (read_bytes == req->content_len) {
            m.cfg->import_json(buffer, req->content_len);
            esp_event_post(APP_EVENT, (int32_t)AppEvent::nvm_update, nullptr, 0, pdMS_TO_TICKS(1));
            free(buffer);
        } else {
            free(buffer);
            httpd_resp_send_500(req);
            return (ESP_FAIL);
        }
    } else {
        httpd_resp_send_500(req);
        return (ESP_FAIL);
    }

    return (ESP_OK);
}


