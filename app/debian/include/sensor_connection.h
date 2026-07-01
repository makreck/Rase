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

#define SECON_DEF_QUERY_TIME_MS         (1000)
#define SECON_MIN_QUERY_TIME_MS         (40)
#define SECON_USE_LAST_QUERY_TIME_MS    (-1)
#define SECON_ERROR_LIMIT               (5)
#define SECON_KEY_NODES                 "\"nodes\""

class SensorBus;
class Datalogger;
class SensorWidget;

class SensorQuery : public MicroJsonObject {
    public:
        char sensor[32]{ 0 };
        char timestamp[20]{ 0 };
        char rssi[16]{ 0 };
        char tx_power[16]{ 0 };
        int channels = 0;

        const MicroJsonStruct meta_data[5] = {
            JSON_ITEM(SensorQuery, sensor,    MicroJsonObjectType::obj_chars),
            JSON_ITEM(SensorQuery, timestamp, MicroJsonObjectType::obj_chars),
            JSON_ITEM(SensorQuery, rssi,      MicroJsonObjectType::obj_chars),
            JSON_ITEM(SensorQuery, tx_power,  MicroJsonObjectType::obj_chars),
            JSON_ITEM(SensorQuery, channels,  MicroJsonObjectType::obj_int),
        };
        JSON_GETTERS(SensorQuery, meta_data);
};

class SensorConnection {
    private:
        struct {
            SensorBus* bus = nullptr;

            char path[64]{ 0 };

            ProductID pid;
            std::vector<Scale*> channels;

            SensorQuery query_header;

            int64_t query_interval_ms = SECON_DEF_QUERY_TIME_MS;
            pthread_t query_thread_handle = 0;

            int initial_count_of_channels = 0;
            int error_count = 0;
            
            Datalogger* datalogger = nullptr;
            SensorWidget* widget = nullptr;
        } m;

        static void* _query_thread(void* _object);
        void query_thread(void);

        int extract_query_data(char* buffer);
        char* parse_query_header(char* key, SensorQuery& query_header);
        char* parse_channel_key(char* key, ScaleJson& sensor_channel);
        void store_channel_data(ScaleJson& sensor_channel);
        void clear_channel_data(void);

    public:
        SensorConnection(SensorBus* _bus, const char* _path, const ProductID* _pid);
        ~SensorConnection();

        void set_path(const char* _path);
        void set_pid(const ProductID* _pid);
        const char* get_path(void);
        const ProductID* get_pid(void);
        bool is_equal_device(SensorConnection* _source);
        bool is_equal_device(const char* _device_serial_number);

        std::vector<Scale*>& get_channels(void);

        void start_query(int64_t _query_interval_ms = SECON_USE_LAST_QUERY_TIME_MS);
        void stop_query(void);
        int query(void);
        void update(void);
        void set_initial_channel_count(int n);
        GtkWidget* set_widget(SensorWidget* _item = nullptr);
        SensorWidget* get_widget(void);
        int get_slot(const char* _node_key);
};
