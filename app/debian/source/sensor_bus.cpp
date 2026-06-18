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

#include "includes.h"

static const char* http_request_format =
    "GET %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "User-Agent: C-Client/1.0\r\n"
    "Accept: %s\r\n"
    "Connection: close\r\n"
    "\r\n";

void SensorBus::init(SensorBusCallback _callback_proc, void* _user_param) {
    m.callback_proc = _callback_proc;
    m.user_param    = _user_param;
    
    pthread_mutex_init(&m.device_list_mutex, nullptr);
    pthread_mutex_init(&m.message_mutex, nullptr);

    memset(m.ip_list, 0, sizeof (m.ip_list));
    m.device_list.clear();
    start_auto_scan();
}

void SensorBus::cleanup(void) {
    stop_auto_scan();
    reset();
    
    pthread_mutex_unlock(&m.device_list_mutex);
    pthread_mutex_destroy(&m.device_list_mutex);

    pthread_mutex_unlock(&m.message_mutex);
    pthread_mutex_destroy(&m.message_mutex);
}

void SensorBus::reset(void) {
    stop_query();

    if (m.scan_thread_handle != 0) {
        pthread_cancel(m.scan_thread_handle);
        pthread_join(m.scan_thread_handle, nullptr);
        m.scan_thread_handle = 0;
    }

    for (SensorConnection*& entry : m.device_list) {
        if (entry != nullptr) {
            delete (entry);
            entry = nullptr;
        }
    }

    memset(m.ip_list, 0, sizeof (m.ip_list));
    m.device_list.clear();
}

int SensorBus::get_available_data_length(int _sock) {
    int length = 0;
    ioctl(_sock, FIONREAD, &length);
    return (length);
}

void SensorBus::set_socket_timeout(int _sock, uint32_t _timeout_ms) {
    uint32_t seconds = _timeout_ms / 1000;
    uint32_t millis  = _timeout_ms % 1000;

    timeval t;
    t.tv_sec = seconds;
    t.tv_usec = millis * 1000;
    setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof (t));
    setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof (t));
}

char* SensorBus::format_request(const char* _host_addr, const char* _json_request, const char* _accept_from) {
    size_t size = (size_t)(snprintf(nullptr, 0, http_request_format, _json_request, _host_addr, _accept_from) + 1);
    char* request = (char*)malloc(size);
    snprintf(request, size, http_request_format, _json_request, _host_addr, _accept_from);
    return (request);
}

ssize_t SensorBus::send_http_request(int _sock, const char* _host_addr, const char* _json_request, const char* _accept_from) {
    char* request = SensorBus::format_request(_host_addr, _json_request, _accept_from);
    size_t size = strlen(request) + 1;
    ssize_t length = send(_sock, request, size, 0);
    free(request);
    return (length);
}

void SensorBus::cleanup_handler(void* arg) {
    int* fd = (int*)arg;
    if (*fd >= 0) {
        close(*fd);
    }
}

SensorConnection* SensorBus::request_id(const char* _host_addr, uint32_t _timeout_ms) {
    SensorConnection* sc = nullptr;
    char* buffer = SensorBus::transact_http_request(_host_addr, SENSOR_API_REQ_ID, SENSOR_REQ_TYPE_JSON, _timeout_ms);
    if (buffer != nullptr) {

        char* json_start = strstr(buffer, "\r\n\r\n");
        if (json_start) {
            json_start += 4;
        } else {
            json_start = buffer;
        }

        ProductID pid;
        if (MicroJson::parse(json_start, &pid)) {
            sc = new SensorConnection(this, _host_addr, &pid);
        }

        free(buffer);
    }

    return (sc);
}

int SensorBus::is_device_registered(const char* _host_addr) {
    for (int i = 0; i < m.device_list.size(); i++) {
        SensorConnection* entry = m.device_list[i];
        if (entry != nullptr) {
            if (strcmp(entry->get_path(), _host_addr) == 0) {
                return (i);
            }
        }
    }
    return (-1);
}

void SensorBus::remove_device(const char* _host_addr) {
    int index = is_device_registered(_host_addr);
    if (index != -1) {
        SensorConnection* entry = m.device_list[index];
        if (entry != nullptr) {
            send_message(SensorBusMessageCode::device_on_remove, (void*)entry);

            pthread_mutex_lock(&m.device_list_mutex);
            m.device_list[index] = nullptr;
            m.device_list.erase(m.device_list.begin() + index);
            pthread_mutex_unlock(&m.device_list_mutex);

            send_message(SensorBusMessageCode::device_removed, (void*)entry);
            set_ip_flag(entry->get_path(), 0x00);
            delete (entry);
        }
    }
}

void SensorBus::start_query(uint64_t _query_interval_ms) {
    send_message(SensorBusMessageCode::query_start, (void*)&m.device_list);
    for (SensorConnection*& entry : m.device_list) {
        if (entry != nullptr) {
            entry->start_query(_query_interval_ms);
        }
    }
    send_message(SensorBusMessageCode::query_started, (void*)&m.device_list);
}

void SensorBus::stop_query(void) {
    send_message(SensorBusMessageCode::query_stop, (void*)&m.device_list);
    for (SensorConnection*& entry : m.device_list) {
        if (entry != nullptr) {
            entry->stop_query();
        }
    }
    send_message(SensorBusMessageCode::query_stopped, (void*)&m.device_list);
}

app_err_t SensorBus::send_message(SensorBusMessageCode _message, void* _arg1, void* _arg2) {
    if (m.callback_proc == nullptr) {
        return (AppState::failed);
    }

    pthread_mutex_lock(&m.message_mutex);
    int result = (*m.callback_proc)(m.user_param, _message, _arg1, _arg2);
    pthread_mutex_unlock(&m.message_mutex);

    return (result);
}

std::vector<SensorConnection*>& SensorBus::aquire_device_list(void) {
    pthread_mutex_lock(&m.device_list_mutex);
    return (m.device_list);
}

void SensorBus::release_device_list(void) {
    pthread_mutex_unlock(&m.device_list_mutex);
}

void SensorBus::set_ip_flag(int _adr, uint8_t _flag) {
    if ((_adr >= 2) && (_adr < 256)) {
        m.ip_list[_adr] = _flag;
    }
}

void SensorBus::set_ip_flag(const char* _path, uint8_t _flag) {
    set_ip_flag(str2ip_index(_path), _flag);
}

uint8_t SensorBus::get_ip_flag(int _adr) {
    if ((_adr >= 2) && (_adr < 256)) {
        return (m.ip_list[_adr]);
    }
    return (0);
}

uint8_t SensorBus::get_ip_flag(const char* _path) {
    return (get_ip_flag(str2ip_index(_path)));
}

app_err_t SensorBus::start_scan(void) {
    if (m.scan_thread_handle != 0) {
        return (AppState::busy);
    }
    pthread_create(&m.scan_thread_handle, nullptr, SensorBus::_scan_ctrl_thread, this);
    return (AppState::ok);
}

void* SensorBus::_scan_ctrl_thread(void* _object) {
    (reinterpret_cast<SensorBus*>(_object))->scan_ctrl_thread();
    return (nullptr);
}
void SensorBus::scan_ctrl_thread(void) {
    send_message(SensorBusMessageCode::scan_begin, (void*)&m.device_list);

    if (!m.partial_scan) {
        stop_query();
    }

    const char* base_ip = "192.168.178.%d";
    pthread_t threads[254]{ 0 };
    int count = 0;
    for (int i = 2; i < 255; i++) {
        usleep(2000);

        if (m.partial_scan) {
            if (get_ip_flag(i) != 0x00) {
                continue;
            }
        }

        char ip_addr[16]{ 0 };
        snprintf(ip_addr, sizeof (ip_addr), base_ip, i);
        
        pthread_t thread_handle = 0;
        if (pthread_create(&thread_handle, nullptr, SensorBus::_scanner_thread, new StaticParameter(this, ip_addr, sizeof (ip_addr))) > -1) {
            threads[count++] = thread_handle;
        }
    }

    if (count > 0) {
        send_message(SensorBusMessageCode::scan_update, (void*)&m.device_list, (void*)(0));
        for (int i = 0; i < count; i++) {
            pthread_join(threads[i], nullptr);
            void* percent = (void*)((int64_t)((float)i * 100.0 / (float)count));
            send_message(SensorBusMessageCode::scan_update, (void*)&m.device_list, percent);
        }
        send_message(SensorBusMessageCode::scan_update, (void*)&m.device_list, (void*)(100));
    }

    send_message(SensorBusMessageCode::scan_complete, (void*)&m.device_list);

    m.scan_thread_handle = 0;
    start_query();
}

void* SensorBus::_scanner_thread(void* _object) {
    StaticParameter* par = (reinterpret_cast<StaticParameter*>(_object));
    SensorBus* p = (reinterpret_cast<SensorBus*>(par->get_this()));
    p->scanner_thread((const char*)par->get_data());
    delete (par);
    return (nullptr);
}
void SensorBus::scanner_thread(const char* _host_addr) {
    SensorConnection* sc = request_id(_host_addr, NETW_RESPONSE_TIMEOUT);
    if (sc != nullptr) {
        if (strstr(sc->get_pid()->identification, SUPPORTED_PRODUCT_TYPE) != nullptr) {
            if (is_device_registered(_host_addr) == -1) {
                int n = sc->query();
                if (n > 0) {
                    set_ip_flag(_host_addr, (uint8_t)n);
                    sc->set_initial_channel_count(n);
                    m.device_list.push_back(sc);
                    send_message(SensorBusMessageCode::device_added, (void*)sc);
                    return;
                }
            }
        }
        delete (sc);
    } else {
        remove_device(_host_addr);
    }
}

int SensorBus::str2ip_index(const char* _path) {
    if (_path != nullptr) {
        const char* p = strstr(_path, ".");
        if (p != nullptr) {
            p = strstr(&p[1], ".");
            if (p != nullptr) {
                p = strstr(&p[1], ".");
                if (p != nullptr) {
                    return (atoi(&p[1]) & 0xff);
                }
            }
        }
    }
    return (-1);
}

void SensorBus::stop_auto_scan(void) {
    if (m.autoscan_thread_handle != 0) {
        pthread_cancel(m.autoscan_thread_handle);
        pthread_join(m.autoscan_thread_handle, nullptr);
        m.autoscan_thread_handle = 0;
    }
}

void SensorBus::start_auto_scan(void) {
    if (m.autoscan_thread_handle == 0) {
        pthread_create(&m.autoscan_thread_handle, nullptr, SensorBus::_autoscan_thread, this);
    }
}

void* SensorBus::_autoscan_thread(void* _object) {
    (reinterpret_cast<SensorBus*>(_object))->autoscan_thread();
    return (nullptr);
}
void SensorBus::autoscan_thread(void) {
    while(true) {
        start_scan();
        sleep(30);
    }
}

char* SensorBus::transact_http_request(const char* _host_addr, const char* _json_request, const char* _accept_from, uint32_t _timeout_ms) {
    char* buffer = nullptr;    

    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return (nullptr);
    }

    pthread_cleanup_push(SensorBus::cleanup_handler, &sock);

    set_socket_timeout(sock, _timeout_ms);

    SockAddrIn serv_addr(_host_addr);
    int result = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (result >= 0) {
        if (SensorBus::send_http_request(sock, _host_addr, _json_request, _accept_from) > 0) {
            buffer = SensorBus::read_http_response(sock, _accept_from, _timeout_ms);
        }
    }

    close(sock);
    pthread_cleanup_pop(0);

    return (buffer);
}

bool SensorBus::poll_socket(int _sock, uint32_t _timeout_ms) {
    struct pollfd pfd;
    pfd.fd = _sock;
    pfd.events = POLLIN;
    int result = poll(&pfd, 1, _timeout_ms * 2);
    return ((result > 0) && (pfd.revents & POLLIN));
}

char* SensorBus::read_http_response(int _sock, const char* _accept_from, uint32_t _timeout_ms) {
    char *buffer = nullptr;    

    if (SensorBus::poll_socket(_sock, _timeout_ms)) {
        int content_len = SensorBus::get_available_data_length(_sock);
        if (content_len < 128) {
            char header[128]{ 0 };
            int read_len = recv(_sock, header, content_len, 0);
            char *p = strstr(header, "Content-Length:");
            if ((strstr(header, _accept_from) != nullptr) && (p != nullptr)) {
                p += 15;
                if ((*p == ' ') || (*p == '\t')) p++;
                content_len = atoi(p) + 2;
            } else {
                return (nullptr);
            }
        }

        buffer = (char *)malloc(content_len + 8);
        if (buffer == nullptr) {
            return (nullptr);
        }
        memset(buffer, 0, content_len + 8);

        int current_len = 0;
        while (current_len < content_len) {
            int remaining_len = content_len - current_len;
            int chunk_size = (remaining_len < 4096) ? remaining_len : 4096;
            int read_len = recv(_sock, &buffer[current_len], chunk_size, 0);
            if (read_len <= 0) {
                break;
            }
            current_len += (size_t)read_len;
        }
        buffer[current_len] = 0;
    }

    return (buffer);
}
