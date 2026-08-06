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

#include "includes.h"

const char* IPDevice::http_request_format =
    "GET %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "User-Agent: C-Client/1.0\r\n"
    "Accept: %s\r\n"
    "Connection: close\r\n"
    "\r\n";

void IPDevice::init(void) {
}

void IPDevice::cleanup(void) {
    wait_for_scan();
    clear_device_list();
}

bool IPDevice::clear_device_list(void) {
    for (DevConfig*& entry : m.device_list) {
        if (entry != nullptr) {
            delete (entry);
            entry = nullptr;
        }
    }
    m.device_list.clear();
    return (true);
}

bool IPDevice::start_scan(void) {
    wait_for_scan();
    clear_device_list();
    pthread_create(&m.scan_thread_handle, nullptr, IPDevice::_scan_ctrl_thread, this);
    return (true);
}

bool IPDevice::wait_for_scan(std::vector<DevConfig*>* device_list) {
    if (m.scan_thread_handle != 0) {
        pthread_join(m.scan_thread_handle, nullptr);
        m.scan_thread_handle = 0;
    }
    if (device_list != nullptr) {
        for (DevConfig*& entry : m.device_list) {
            if (entry != nullptr) {
                device_list->push_back(entry);
                entry = nullptr;
            }
        }
        m.device_list.clear();
    }
    return (true);
}

void* IPDevice::_scan_ctrl_thread(void* _object) {
    (reinterpret_cast<IPDevice*>(_object))->scan_ctrl_thread();
    return (nullptr);
}
void IPDevice::scan_ctrl_thread(void) {
    const char* base_ip = "192.168.178.%d"; // FritzBox only

    pthread_t threads[254]{ 0 };
    int count = 0;
    for (int i = 2; i < 255; i++) {
        usleep(2000);

        char ip_addr[16]{ 0 };
        snprintf(ip_addr, sizeof (ip_addr), base_ip, i);
        
        pthread_t thread_handle = 0;
        if (pthread_create(&thread_handle, nullptr, IPDevice::_scanner_thread, new StaticParameter(this, ip_addr, sizeof (ip_addr))) > -1) {
            threads[count++] = thread_handle;
        }
    }

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            pthread_join(threads[i], nullptr);
        }
    }
}

void* IPDevice::_scanner_thread(void* _object) {
    StaticParameter* par = (reinterpret_cast<StaticParameter*>(_object));
    IPDevice* p = (reinterpret_cast<IPDevice*>(par->get_this()));
    p->scanner_thread((const char*)par->get_data());
    delete (par);
    return (nullptr);
}
void IPDevice::scanner_thread(const char* _host_addr) {
    DevConfig* device = new DevConfig();

    char* id_buffer = IPDevice::transact_http_request(_host_addr, WEB_KEY_ID_RESPONSE, SENSOR_REQ_TYPE_JSON, NETW_RESPONSE_TIMEOUT);
    if (id_buffer != nullptr) {
        char* json_start = get_http_json_payload_begin(id_buffer);
        device->parse_id_json(json_start);
        free(id_buffer);
    }

    char* cfg_buffer = IPDevice::transact_http_request(_host_addr, WEB_KEY_CONFIG_API, SENSOR_REQ_TYPE_JSON, NETW_RESPONSE_TIMEOUT);
    if (cfg_buffer != nullptr) {
        char* json_start = get_http_json_payload_begin(cfg_buffer);
        device->parse_config_json(json_start);
        free(cfg_buffer);
    }

    if (strlen(device->id.device_serial_number) > 0) {
        snprintf(device->ifac, sizeof (device->ifac), "http://%s", _host_addr);
        m.device_list.push_back(device);
    } else {
        delete (device);
    }
}

char* IPDevice::get_http_json_payload_begin(char *buffer) {
    char *json_start = strstr(buffer, "\r\n\r\n");
    if (json_start) {
        json_start += 4;
    } else {
        json_start = buffer;
    }
    return (json_start);
}

void IPDevice::cleanup_handler(void* arg) {
    int* fd = (int*)arg;
    if (*fd >= 0) {
        close(*fd);
    }
}

void IPDevice::set_socket_timeout(int _sock, uint32_t _timeout_ms) {
    uint32_t seconds = _timeout_ms / 1000;
    uint32_t millis  = _timeout_ms % 1000;

    timeval t;
    t.tv_sec = seconds;
    t.tv_usec = millis * 1000;
    setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof (t));
    setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof (t));
}

bool IPDevice::poll_socket(int _sock, uint32_t _timeout_ms) {
    struct pollfd pfd;
    pfd.fd = _sock;
    pfd.events = POLLIN;
    int result = poll(&pfd, 1, _timeout_ms);
    return ((result > 0) && (pfd.revents & POLLIN));
}

int IPDevice::get_available_data_length(int _sock) {
    int length = 0;
    ioctl(_sock, FIONREAD, &length);
    return (length);
}

char* IPDevice::format_request(const char* _host_addr, const char* _json_request, const char* _accept_from) {
    size_t size = (size_t)(snprintf(nullptr, 0, http_request_format, _json_request, _host_addr, _accept_from) + 1);
    char* request = (char*)malloc(size);
    snprintf(request, size, IPDevice::http_request_format, _json_request, _host_addr, _accept_from);
    return (request);
}

ssize_t IPDevice::send_http_request(int _sock, const char* _host_addr, const char* _json_request, const char* _accept_from) {
    char* request = IPDevice::format_request(_host_addr, _json_request, _accept_from);
    size_t size = strlen(request) + 1;
    ssize_t length = send(_sock, request, size, 0);
    free(request);
    return (length);
}

char* IPDevice::read_http_headers(int _sock, uint32_t _timeout_ms) {
    int headers_len = IPDevice::get_available_data_length(_sock);
    size_t size = headers_len + 2;
    char* headers = (char *)malloc(size);
    if (headers == nullptr) {
        return (nullptr);
    }
    memset(headers, 0, size);

    char data = 0;
    int read_len = 0;
    while ((recv(_sock, &data, 1, 0) == 1) && (read_len < headers_len)) {
        headers[read_len++] = data;
        if (read_len > 4) {
            if (!memcmp(&headers[read_len - 4], "\r\n\r\n", 4)) {
                break;
            }
        }
    }

    return (headers);
}

int IPDevice::parse_http_headers(char* _headers, const char* _accept_from) {
    int content_len = 0;
    char *p = strstr(_headers, "Content-Length:");
    if ((strstr(_headers, _accept_from) != nullptr) && (p != nullptr)) {
        p += 15;
        if ((*p == ' ') || (*p == '\t')) {
            p++; 
        }
        content_len = atoi(p) + 2;
    }
    return (content_len);
}

char* IPDevice::read_http_body(int _sock, int _content_len) {
    size_t size = _content_len + 2;
    char* buffer = (char *)malloc(size);
    if (buffer == nullptr) {
        return (nullptr);
    }
    memset(buffer, 0, size);

    int current_len = 0;
    while (current_len < _content_len) {
        int remaining_len = _content_len - current_len;
        int chunk_size = (remaining_len < 4096) ? remaining_len : 4096;
        int read_len = recv(_sock, &buffer[current_len], chunk_size, 0);
        if (read_len <= 0) {
            break;
        }
        current_len += read_len;
    }

    return (buffer);
}

char* IPDevice::read_http_response(int _sock, const char* _accept_from, uint32_t _timeout_ms) {
    if (!poll_socket(_sock, _timeout_ms)) {
        return (nullptr);
    }

    char* headers = read_http_headers(_sock, _timeout_ms);
    if (headers == nullptr) {
        return (nullptr);
    }

    int content_len = parse_http_headers(headers, _accept_from);
    free(headers);
    if (content_len < 1) {
        return (nullptr);
    }

    return (read_http_body(_sock, content_len));
}

char* IPDevice::transact_http_request(const char* _host_addr, const char* _json_request, const char* _accept_from, uint32_t _timeout_ms) {
    char* buffer = nullptr;    

    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return (nullptr);
    }

    pthread_cleanup_push(IPDevice::cleanup_handler, &sock);

    set_socket_timeout(sock, _timeout_ms);

    SockAddrIn serv_addr(_host_addr);
    int result = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (result >= 0) {
        if (IPDevice::send_http_request(sock, _host_addr, _json_request, _accept_from) > 0) {
            buffer = IPDevice::read_http_response(sock, _accept_from, _timeout_ms);
        }
    }

    close(sock);
    pthread_cleanup_pop(0);

    return (buffer);
}
