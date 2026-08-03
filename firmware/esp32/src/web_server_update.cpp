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


esp_err_t WebServer::_update_root_handler(httpd_req_t* req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->update_root_handler(req));
}
esp_err_t WebServer::update_root_handler(httpd_req_t* req) {

#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::update_root_handler() event.");
#endif
    const char* website = WebServer::firmware_update_resp_str;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, website, HTTPD_RESP_USE_STRLEN);

    esp_event_post(APP_EVENT, (int32_t)AppEvent::web_query_event, nullptr, 0, pdMS_TO_TICKS(100));
    return (ESP_OK);
}


esp_err_t WebServer::_update_put_handler(httpd_req_t* req) {
    return ((reinterpret_cast<WebServer*>(req->user_ctx))->update_put_handler(req));
}
esp_err_t WebServer::update_put_handler(httpd_req_t* req) {

#ifdef DISPLAY_STATE
    ESP_LOGI(TAG, "WebServer::update_put_handler() event. OTA request, content length = %.1f KB.", (float)req->content_len / 1024.0f);
#endif

    if (req->content_len == 0) {
        httpd_resp_send_err(req, HTTPD_411_LENGTH_REQUIRED, nullptr);
        return (ESP_FAIL);
    }

    const esp_partition_t* update_partition = Tools::get_next_ota_partition();
    if (!update_partition) {
        httpd_resp_send_500(req);
        return (ESP_FAIL);
    }

    uint8_t* chunk_buf = (uint8_t*)malloc(OTA_CHUNK_SIZE);
    if (chunk_buf == nullptr) {
        httpd_resp_send_500(req);
        return (ESP_FAIL);
    }

    if (m.display != nullptr) {
        esp_event_post(APP_EVENT, (int32_t)AppEvent::display_lock, nullptr, 0, pdMS_TO_TICKS(1));
        m.display->clear();
        m.display->print( 0, 0, "Firmware upload:");
        m.display->print( 0, 1, "Partition:");
        m.display->print(10, 1, update_partition->label);
        char string[20]{ 0 };
        snprintf(string, sizeof (string), "Size:  %9.9zu", req->content_len);
        m.display->print( 0, 2, string);
        m.display->print( 0, 3, "State: Ereasing");
        m.display->update();
    }

    esp_ota_mark_app_valid_cancel_rollback();

    size_t received = 0;
    esp_ota_handle_t ota_handle = -1;
    esp_err_t ret = esp_ota_begin(update_partition, req->content_len, &ota_handle);
    if (ret == ESP_OK) {
        float percent = 0.0f;
        while (true) {
            int read_bytes = httpd_req_recv(req, (char*)chunk_buf, OTA_CHUNK_SIZE);
            if (read_bytes < 0) {
                ret = ESP_ERR_INVALID_STATE;
                break;
            }

            if (read_bytes == 0) {
                ret = ESP_OK;
                break;
            }

            ret = esp_ota_write(ota_handle, chunk_buf, read_bytes);
            if (ret != ESP_OK) {
                break;
            }

            received += read_bytes;

            float progress = (float)received * 100.0f / (float)req->content_len;
            if ((fabs(progress - percent) >= 5.0f) || (progress == 0.0f)) {
                percent = progress;
                if (m.display != nullptr) {
                    char string[20]{ 0 };
                    snprintf(string, 16, "%9.9zu", received);
                    m.display->print( 7, 3, string);
                    m.display->update();
                }
            }
        }
    }

    free(chunk_buf);

    if ((ret == ESP_OK) && (received != req->content_len)) {
        esp_ota_abort(ota_handle);
        ret = ESP_ERR_INVALID_SIZE;
    } else {
        ret = esp_ota_end(ota_handle);
        if (ret == ESP_OK) {
            ret = esp_ota_set_boot_partition(update_partition);
        }
    }

    AppEvent event_code;
    if (ret == ESP_OK) {
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, WebServer::end_of_ota_update, HTTPD_RESP_USE_STRLEN);
        event_code = AppEvent::reboot;
    } else {
        httpd_resp_send_500(req);
        event_code = AppEvent::display_unlock;
    }

    if (m.display != nullptr) {
        char state_txt[12]{ 0 };
        snprintf(state_txt, sizeof (state_txt), "Error %-3.3X", (unsigned int)ret);
        const char* state = (ret == ESP_OK) ? "OK       " : state_txt;
        m.display->print(7, 3, state);
        m.display->update();
        vTaskDelay(pdMS_TO_TICKS(2500));
    }

    esp_event_post(APP_EVENT, (int32_t)event_code, nullptr, 0, pdMS_TO_TICKS(1));
    return (ESP_OK);
}
