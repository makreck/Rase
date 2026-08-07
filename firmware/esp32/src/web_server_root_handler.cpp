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


esp_err_t WebServer::_root_handler(httpd_req_t* req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->root_handler(req));
}
esp_err_t WebServer::root_handler(httpd_req_t* req) {
#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::root_handler() event.");
#endif

    const char* website = nullptr;
    if (strcmp(req->uri, WEB_KEY_UPDATE_ROOT) == 0) {
        website = WebServer::firmware_update_resp_str;
    } else if (strcmp(req->uri, WEB_KEY_CONFIG_ROOT) == 0) {
        website = WebServer::config_website_resp_str;
    } else {
        website = WebServer::webserver_resp_str_1;
    }

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, website, HTTPD_RESP_USE_STRLEN);

    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_query_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}
