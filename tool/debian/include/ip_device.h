/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,      Preliminary Configuration Tool
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

#define NETW_HTTP_PORT              (80)
#define NETW_RESPONSE_TIMEOUT       (2000)
#define SENSOR_REQ_TYPE_JSON        "application/json"
#define OTA_CHUNK_SIZE              (4096)

class PartitionEntry {
    public:
        const char* name;
        uint8_t     type;
        uint8_t     subtype;
        uint32_t    offset;
        uint32_t    size;
        uint32_t    flags;
};

class SockAddrIn : public sockaddr_in {
    public:
        SockAddrIn(void) {
            memset(this, 0, sizeof (sockaddr_in));
        }

        SockAddrIn(const char* _host_addr) {
            import(_host_addr);
        }

        int import(const char* _host_addr) {
            memset(this, 0, sizeof (sockaddr_in));
            this->sin_family = AF_INET;
            this->sin_port = htons(NETW_HTTP_PORT);
            return (inet_pton(AF_INET, _host_addr, &this->sin_addr));
        }
};

class IPDevice {
    private:
        struct {
            pthread_t scan_thread_handle = 0;
            pthread_mutex_t* device_list_mutex = nullptr;
            std::vector<DevConfig*>* device_list = nullptr;
        } m;

        static const char* http_request_format;
        static const char* ota_put_req_string;
        static const PartitionEntry partitions[6];

        void init(void);
        void cleanup(void);
        
        static void* _scan_ctrl_thread(void* _object);
        void scan_ctrl_thread(void);
        
        static void* _scanner_thread(void* _object);
        void scanner_thread(const char* _host_addr);

        static char* get_http_json_payload_begin(char *buffer);
        static bool  poll_socket(int _sock, uint32_t _timeout_ms);
        static int   get_available_data_length(int _sock);
        static void  cleanup_handler(void* arg);
        static bool  ota_loader(const char* _ip_addr, uint8_t* _firmware_image, ssize_t _size, OTALoaderCB _callback, void* _user_param);
        static void* _loader_thread(void* _object);

        void    set_socket_timeout(int _sock, uint32_t _timeout_ms);
        char*   format_request(const char* _host_addr, const char* _json_request, const char* _accept_from);
        ssize_t send_http_request(int _sock, const char* _host_addr, const char* _json_request, const char* _accept_from);
        char*   read_http_headers(int _sock, uint32_t _timeout_ms);
        int     parse_http_headers(char* _headers, const char* _accept_from);
        char*   read_http_body(int _sock, int _content_len);
        char*   read_http_response(int _sock, const char* _accept_from, uint32_t _timeout_ms);
        char*   transact_http_request(const char* _host_addr, const char* _json_request, const char* _accept_from, uint32_t _timeout_ms);

    public:
        IPDevice() {
            init();
        }

        ~IPDevice() {
            cleanup();
        }

        static pthread_t firmware_loader(const char* _ifac, const char* _filename, OTALoaderCB _callback = nullptr, void* _user_param = nullptr);

        bool start_scan(std::vector<DevConfig*>* _device_list, pthread_mutex_t* _device_list_mutex);
        bool wait_for_scan(void);

};
