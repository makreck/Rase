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

class SensorDevice {
    private:
        static const char* sensor_header;
        static const char* opcua_header;
        static const char* end_of_list;

        struct {
            char device_name[32]{ 0 };
            SemaphoreHandle_t mutex = nullptr;
            std::vector<SensorNode*> node;
            float rec_interval_s = 0.01f;

            SensorDriver* driver = nullptr;

            char* json = nullptr;
            size_t length = 0;

            char* opcua_json = nullptr;
            size_t opcua_length = 0;
        } m;

        void update_json(void);
        void update_opcua_json(void);

    public:
        SensorDevice(const char* _device_name = SENSOR_ID ".ESP32");
        ~SensorDevice();

        void set_driver(SensorDriver* driver);
        SensorDriver* get_driver(void);
        SensorNode* get_channel(size_t index);
        size_t get_channel_count(void);
        bool get_channel_props(size_t index, SensorNodeProps& props, bool modify_unit = true);
        void delete_driver(void);
        void add_channel(SensorNode* channel);
        void update(SensorReading* reading);

        char* get_json(size_t& length);
        char* get_opcua_json(size_t& length);
};
