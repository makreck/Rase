/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
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
#define NETW_DEF_QUERY_INTERVAL_MS  (1000)

#define SENSOR_API_REQ_SENSORS      "/api/sensors"
#define SENSOR_API_REQ_ID           "/api/id"
#define SENSOR_REQ_TYPE_JSON        "application/json"

enum class SensorBusMessageCode {
    scan_begin         = 10,
    scan_update        = 11,
    scan_complete      = 12,

    device_added       = 20,
    device_on_remove   = 21,
    device_removed     = 22,

    query_start        = 30,
    query_started      = 31,
    query_stop         = 32,
    query_stopped      = 33,

    new_measurement    = 40,
    rescan_request     = 41,
    
    recording_started  = 50,
    recording_ended    = 51,
};

typedef app_err_t (*SensorBusCallback)(void* _user_param, SensorBusMessageCode _message, void* _arg1, void* _arg2);

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

class SensorBus {
    private:
        struct {
            SensorBusCallback callback_proc = nullptr;
            void* user_param = nullptr;

            std::vector<SensorConnection*> device_list;
            pthread_t scan_thread_handle = 0;
            pthread_t autoscan_thread_handle = 0;

            pthread_mutex_t device_list_mutex = PTHREAD_MUTEX_INITIALIZER;
            pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;

            bool partial_scan = true;
            uint8_t ip_list[256]{ 0 };
        } m;

        void init(SensorBusCallback _callback_proc, void* _user_param);
        void cleanup(void);

        static void* _autoscan_thread(void* _object);
        void autoscan_thread(void);

        static void* _scan_ctrl_thread(void* _object);
        void scan_ctrl_thread(void);

        static void* _scanner_thread(void* _object);
        void scanner_thread(const char* _host_addr);

        static void cleanup_handler(void* arg);


    public:
        SensorBus(SensorBusCallback _callback_proc, void* _user_param) {
            init(_callback_proc, _user_param);
        }

        ~SensorBus() {
            cleanup();
        }

        static void set_socket_timeout(int _sock, uint32_t _timeout_ms);
        static char* format_request(const char* _host_addr, const char* _json_request, const char* _accept_from);
        static char* transact_http_request(const char* _host_addr, const char* _json_request, const char* _accept_from, uint32_t _timeout_ms);
        static char* read_http_response(int sock, const char* _accept_from, uint32_t _timeout_ms);
        static int get_available_data_length(int _sock);
        static ssize_t send_http_request(int sock, const char* _host_addr, const char* _json_request, const char* _accept_from);
        static bool poll_socket(int sock, uint32_t _timeout_ms);

        void set_ip_flag(int _adr, uint8_t _flag);
        void set_ip_flag(const char* _path, uint8_t _flag);
        uint8_t get_ip_flag(int _adr);
        uint8_t get_ip_flag(const char* _path);
        int str2ip_index(const char* _path);

        void start_auto_scan(void);
        void stop_auto_scan(void);
        void start_query(uint64_t _query_interval_ms = NETW_DEF_QUERY_INTERVAL_MS);
        void stop_query(void);
        void reset(void);
        void remove_device(const char* _host_addr);
        int is_device_registered(const char* _host_addr);
        SensorConnection* request_id(const char* _host_addr, uint32_t _timeout_ms);

        app_err_t start_scan(void);
        app_err_t send_message(SensorBusMessageCode _message, void* _arg1 = nullptr, void* _arg2 = nullptr);

        std::vector<SensorConnection*>& aquire_device_list(void);
        void release_device_list(void);

};
