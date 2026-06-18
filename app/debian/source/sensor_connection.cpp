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

SensorConnection::SensorConnection(SensorBus* _bus, const char *_path, const ProductID *_pid) {
    m.bus = _bus;
    set_path(_path);
    set_pid(_pid);
}

SensorConnection::~SensorConnection() {
    stop_query();
    clear_channel_data();
}

void SensorConnection::set_path(const char *_path) {
    memset(m.path, 0, sizeof (m.path));
    if (_path != nullptr) {
        strncpy(m.path, _path, sizeof (m.path) - 1);
    }
}

void SensorConnection::set_pid(const ProductID *_pid) {
    if (_pid != nullptr) {
        memcpy(&m.pid, _pid, sizeof (m.pid));
    } else {
        memset(&m.pid, 0, sizeof (m.pid));
    }
}

const char* SensorConnection::get_path(void) {
    return (m.path);
}

const ProductID* SensorConnection::get_pid(void) {
    return (&m.pid);
}

std::vector<Scale*>& SensorConnection::get_channels(void) {
    return (m.channels);
}

bool SensorConnection::is_equal_device(SensorConnection *_source) {
    return (m.pid.is_equal_device(_source->get_pid()));
}

void SensorConnection::stop_query(void) {
    if (m.query_thread_handle != 0) {
        pthread_cancel(m.query_thread_handle);
        pthread_join(m.query_thread_handle, nullptr);
        m.query_thread_handle = 0;
    }

    if (m.datalogger != nullptr) {
        m.bus->send_message(SensorBusMessageCode::recording_ended, (void*)m.datalogger->get_path());
        delete (m.datalogger);
        m.datalogger = nullptr;
    }
}

void SensorConnection::start_query(int64_t _query_interval_ms) {
    m.error_count = 0;
    
    if (_query_interval_ms < 0) {
        if (m.query_interval_ms <= SECON_MIN_QUERY_TIME_MS) {
            m.query_interval_ms = SECON_DEF_QUERY_TIME_MS;
        }
    } else {
        m.query_interval_ms = _query_interval_ms;
    }

    if (m.query_thread_handle == 0) {
        pthread_create(&m.query_thread_handle, nullptr, SensorConnection::_query_thread, this);
    }
}

void* SensorConnection::_query_thread(void* _object) {
    (reinterpret_cast<SensorConnection*>(_object))->query_thread();
    return (nullptr);
}
void SensorConnection::query_thread(void) {
    uint64_t time_stamp = Times::get_tick_count64() + (uint64_t)m.query_interval_ms;
    while (true) {
        usleep(20000);
        if (Times::get_tick_count64() >= time_stamp) {
            time_stamp += (uint64_t)m.query_interval_ms;

            int n = query();
            if (n != m.initial_count_of_channels) {
                m.error_count++;
                if (m.error_count == SECON_ERROR_LIMIT) {
                    m.error_count++;
                    m.bus->set_ip_flag(get_path(), 0x00);
                    m.bus->send_message(SensorBusMessageCode::rescan_request, (void*)this);
                }
            } else {
                m.error_count = 0;
            }
        }
    }
}

void SensorConnection::clear_channel_data(void) {
    for (Scale*& channel : m.channels) {
        if (channel != nullptr) {
            ChannelWidget* widget = (ChannelWidget*)channel->get_userdata();
            if (widget != nullptr) {
                channel->set_userdata(nullptr);
                delete (widget);
            }
            delete (channel);
            channel = nullptr;
        }
    }
    m.channels.clear();
}

int SensorConnection::query(void) {
    int n = -1;

    char* buffer = SensorBus::transact_http_request(get_path(), SENSOR_API_REQ_SENSORS, SENSOR_REQ_TYPE_JSON, 100);
    if (buffer != nullptr) {
        n = extract_query_data(buffer);
        free(buffer);
    }
    
    return (n);
}

void SensorConnection::update(void) {
    for (Scale*& channel_data : m.channels) {
        ChannelWidget* widget = (ChannelWidget*)channel_data->get_userdata();
        if (widget != nullptr) {
            widget->update();
        }
    }
}

void SensorConnection::set_initial_channel_count(int n) {
    if ((n > 0) && (n < 7)) {
        m.initial_count_of_channels = n;
    }
}

char* SensorConnection::parse_query_header(char* buffer, SensorQuery& query_header) {
    char* key = strstr(buffer, SECON_KEY_NODES);
    if (key == nullptr) { // *** check for depreciated old key also
        key = strstr(buffer, SECON_KEY_CHANNELS);
    }
    if (key == nullptr) return (nullptr);

    char* p = key;
    while ((p != buffer) && (*p != ',')) p--;
    *p++ = '}';
    *p++ = 0;

    MicroJson::parse(buffer, &m.query_header);

    return (strstr(p, "["));
}

char* SensorConnection::parse_channel_key(char* key, ScaleJson& sensor_channel) {
    char* p = strstr(key, "{");
    if (p == nullptr) return (nullptr);
    p = strstr(p, "\"");
    if (p == nullptr) return (nullptr);

    p++;
    for (int i = 0; (i < (sizeof (sensor_channel.key) - 1)) && (*p != '\"'); i++) {
        sensor_channel.key[i] = *p++;
    }

    key = strstr(p, "{");
    if (key == nullptr) return (nullptr);

    p = strstr(key, "}");
    if (p == nullptr) return (nullptr);
    p++;
    *p++ = 0;

    MicroJson::parse(key, &sensor_channel);

    return (p);
}

int SensorConnection::extract_query_data(char* buffer) {
    if (buffer == nullptr) {
        return (-1);
    }

    char* key = parse_query_header(buffer, m.query_header);
    if (key == nullptr) {
        return (-1);
    }

    int channel_count = 0;
    do {
        ScaleJson sensor_channel;
        key = parse_channel_key(key, sensor_channel);
        if (key != nullptr) {
            channel_count++;
            store_channel_data(sensor_channel);
        }
    } while (key != nullptr);

    if (m.datalogger == nullptr) {
        m.datalogger = new Datalogger(nullptr, &m.pid, &m.channels);
        m.bus->send_message(SensorBusMessageCode::recording_started, (void*)m.datalogger->get_path());
    } else {
        m.datalogger->add_measurement(&m.channels);
    }

    m.bus->send_message(SensorBusMessageCode::new_measurement, (void*)this);

    return (channel_count);
}

void SensorConnection::store_channel_data(ScaleJson& json_scale) {
    Scale sensor_channel(&json_scale);
    for (Scale*& channel : m.channels) {
        if (channel != nullptr) {
            if (strncmp(channel->key, sensor_channel.key, sizeof (channel->key)) == 0) {
                if (channel->set_values(&sensor_channel)) {
                    ChannelWidget* widget = (ChannelWidget*)channel->get_userdata();
                    if (widget != nullptr) {
                        widget->request_update();
                    }
                }
                return;
            }
        }
    }
    Scale* new_channel = new Scale(sensor_channel);
    m.channels.push_back(new_channel);
}
