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

#define CONFIG_SNTP_SERVER_0 "pool.ntp.org"
#define CONFIG_SNTP_SERVER_1 "time.google.com"
#define CONFIG_SNTP_SERVER_2 "time.cloudflare.com"

#define OTA_CHUNK_SIZE (4096)

class SensorDevice;
class DisplayI2C;
class SysConfig;

class WebServerURI {
    public:
        const char* http_uri;
        httpd_method_t method;
        esp_err_t (*handler)(httpd_req_t* _req);
};

class WebServer {
    private:
        const char* sensor_header = 
            "{\n"
            "\t\"sensor\": \"%s\",\n"
            "\t\"channels:\"\n"
            "\t\"{\"\n";

        const char* sensor_tail = 
            "\t}\n"
            "}\n";

        const char* end_of_ota_update =
            "http\n"
            "HTTP/1.1 200 OK\n"
            "Content-Type: application/json\n"
            "Content-Length: 58\n"
            "Connection: close\n"
            "\r\n"
            "{"
            "\t\"status\": \"success\",\n"
            "\t\"message\": \"OTA update complete\"\n"
            "}\n";

        static const WebServerURI web_uri_tab[];

        struct {
            httpd_config_t config  = HTTPD_DEFAULT_CONFIG();
            httpd_handle_t server  = nullptr;

            SensorDevice*  sensor  = nullptr;
            DisplayI2C*    display = nullptr;
            SysConfig*     cfg     = nullptr;
            
            char           ip_addr[32]{ 0 };
        } m;

        static esp_err_t _root_handler(httpd_req_t *req);
        esp_err_t root_handler(httpd_req_t *req);

        static esp_err_t _api_sensors_handler(httpd_req_t *req);
        esp_err_t api_sensors_handler(httpd_req_t *req);

        static esp_err_t _api_id_handler(httpd_req_t *req);
        esp_err_t api_id_handler(httpd_req_t *req);

        static esp_err_t _update_root_handler(httpd_req_t *req);
        esp_err_t update_root_handler(httpd_req_t *req);

        static esp_err_t _update_put_handler(httpd_req_t *req);
        esp_err_t update_put_handler(httpd_req_t *req);

        static esp_err_t _config_root_handler(httpd_req_t *req);
        esp_err_t config_root_handler(httpd_req_t *req);

        static esp_err_t _api_cfg_get_handler(httpd_req_t *req);
        esp_err_t api_cfg_get_handler(httpd_req_t *req);

        static esp_err_t _api_cfg_put_handler(httpd_req_t* req);
        esp_err_t api_cfg_put_handler(httpd_req_t* req);

        static void _time_sync_notification(struct timeval* tv);

        esp_err_t init(void);
        esp_err_t cleanup(void);
        esp_err_t init_time_server(void);

    public:
        static const char* webserver_resp_str_1;
        static const char* config_website_resp_str;

        WebServer(void) {
            init();
        }

        ~WebServer() {
            cleanup();
        }

        esp_err_t start(App* _app, const char* ip_addr = "0.0.0.0");
        esp_err_t stop(void);
        esp_err_t update(SensorReading* reading);
};
