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

const PartitionEntry IPDevice::partitions[6] = {
    { "nvs",      0x01, 0x00, 0x009000, 0x005000, 0 },
    { "otadata",  0x01, 0x01, 0x00e000, 0x002000, 0 },
    { "app0",     0x00, 0x00, 0x010000, 0x180000, 0 },
    { "app1",     0x00, 0x01, 0x190000, 0x180000, 0 },
    { "spiffs",   0x01, 0x02, 0x310000, 0x0E0000, 0 },
    { "coredump", 0x01, 0x03, 0x3F0000, 0x010000, 0 }
};

const char* IPDevice::ota_put_req_string =
    "PUT %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Content-Length: %ld\r\n"
    "Connection: close\r\n"
    "\r\n";

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
}

bool IPDevice::start_scan(std::vector<DevConfig*>* _device_list, pthread_mutex_t* _device_list_mutex) {
    m.device_list       = _device_list;
    m.device_list_mutex = _device_list_mutex;

    wait_for_scan();
    pthread_create(&m.scan_thread_handle, nullptr, IPDevice::_scan_ctrl_thread, this);

    return (true);
}

bool IPDevice::wait_for_scan(void) {
    if (m.scan_thread_handle != 0) {
        pthread_join(m.scan_thread_handle, nullptr);
        m.scan_thread_handle = 0;
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

bool IPDevice::ota_loader(const char* _ip_addr, uint8_t* _firmware_image, ssize_t _size, OTALoaderCB _callback, void* _user_param) {
    const char* topic = "OTA update";

    struct addrinfo hints{ 0 };
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (_callback != nullptr) {
        const char* p = "Connect...";
        (*_callback)(_user_param, topic, p);
    }

    struct addrinfo* res = nullptr; 
    int s = getaddrinfo(_ip_addr, "80", &hints, &res);
    if (s != 0) {
        return (true);
    }

    int fd = -1;
    for (struct addrinfo* rp = res; rp != nullptr; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1) {
            continue;
        }
        if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(fd);
        fd = -1;
    }

    freeaddrinfo(res);
    if (fd == -1) {
        return (false);
    }

    ssize_t req_len = snprintf(nullptr, 0, ota_put_req_string, WEB_KEY_API_UPDATE, _ip_addr, _size);
    char* request = (char*)malloc(req_len + 2);
    if (request == nullptr) {
        return (false);
    }
    snprintf(request, req_len + 1, ota_put_req_string, WEB_KEY_API_UPDATE, _ip_addr, _size);

    ssize_t sent_len = send(fd, request, req_len, 0);
    free(request);
    if (sent_len != req_len) {
        close(fd);
        return (false);
    }

    ssize_t i = 0;
    float percent = 0.0f;
    while (i < _size) {
        ssize_t chunk_len = ((_size - i) > OTA_CHUNK_SIZE) ? OTA_CHUNK_SIZE : (_size - i);
        sent_len = send(fd, &_firmware_image[i], chunk_len, 0);
        if (sent_len == -1) {
            close(fd);
            return (false);
        }
        i += sent_len;

        float progress = (float)i * 100.0f / (float)_size;
        if (fabsf(progress - percent) >= 5.0f) {
            percent = progress;
            if (_callback != nullptr) {
                char string[64]{ 0 };
                snprintf(string, sizeof (string), "%.0f%%, %zu / %zu", percent, i, _size);
                (*_callback)(_user_param, topic, string);
            }
        }
    }

    close(fd);

    if (_callback != nullptr) {
        const char* p = "Complete.";
        (*_callback)(_user_param, topic, p);
    }

    return (true);
}

pthread_t IPDevice::firmware_loader(const char* _ifac, const char* _filename, OTALoaderCB _callback, void* _user_param) {
    pthread_t thread_handle = 0;
    pthread_create(&thread_handle, nullptr, IPDevice::_loader_thread, new OTAParms(_ifac, _filename, _callback, _user_param));
    return (thread_handle);
}
void* IPDevice::_loader_thread(void* _object) {
    OTAParms* parms = (OTAParms*)_object;
    if ((parms->image_data != nullptr) && (parms->image_length > 0)) {
        IPDevice::ota_loader(parms->ifac, parms->image_data, parms->image_length, parms->callback, parms->user_param);
    }
    delete (parms);
    return (nullptr);
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

    char* id_buffer = IPDevice::transact_http_request(_host_addr, WEB_KEY_API_IDENTIFY, SENSOR_REQ_TYPE_JSON, NETW_RESPONSE_TIMEOUT);
    if (id_buffer != nullptr) {
        char* json_start = get_http_json_payload_begin(id_buffer);
        device->parse_id_json(json_start);
        free(id_buffer);
    }

    char* cfg_buffer = IPDevice::transact_http_request(_host_addr, WEB_KEY_API_CONFIG, SENSOR_REQ_TYPE_JSON, NETW_RESPONSE_TIMEOUT);
    if (cfg_buffer != nullptr) {
        char* json_start = get_http_json_payload_begin(cfg_buffer);
        device->parse_config_json(json_start);
        free(cfg_buffer);
    }

    if (strlen(device->id.device_serial_number) > 0) {
        device->set_ip_interface(_host_addr);
        pthread_mutex_lock(m.device_list_mutex); {
            if (!device->register_device(m.device_list)) {
                delete (device);
            }
        } pthread_mutex_unlock(m.device_list_mutex);
    } else {
        delete (device);
    }
}
