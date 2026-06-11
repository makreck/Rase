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

#define SENSOR_STD_READING_INTERVAL_MS (500)
#define SENSOR_STD_RETRY_DELAY_MS (100)
#define SENSOR_SUSPEND_DELAY_MS (1000)
#define SENSOR_SERIAL_NUMBER_NOT_SUPPORTED (0x12345678)

#define SP_FLAGS(dot_num, line_width, f_sign, f_reverse)   ((uint32_t)(((int)(dot_num)    & 0x0f) << 0) | \
                                                            (uint32_t)(((int)(line_width) & 0x0f) << 4) | \
                                                            (uint32_t)(((int)(f_sign)     & 0x01) << 8) | \
                                                            (uint32_t)(((int)(f_reverse)  & 0x01) << 9) )

class SensorProperty {
    public:
        const char* key;
        const char* name;
        const char* shortcut;
        const char* unit;
        float bottom;
        float top;
        uint32_t flags;
        const char* color;
        int node_id;
};

class SensorValue {
    public:
        const char* key;
        float value;

        SensorValue(const char* _key, float _initial_value = 0.0f) {
            key = _key;
            value = _initial_value;
        }
};

class SensorReading {
    public:

        std::vector<SensorValue*> data;

        SensorReading() {
        }

        ~SensorReading() {
            for (int i = 0; i < data.size(); i++) {
                delete (data[i]);
            }
            data.clear();
        }

        void set_Value(const char* key, float value) {
            for (int i = 0; i < data.size(); i++) {
                if (!strcmp(data[i]->key, key)) {
                    data[i]->value = value;
                    return;
                }
            }
        }

        bool get_Value(const char* key, float& value) {
            for (int i = 0; i < data.size(); i++) {
                if (!strcmp(data[i]->key, key)) {
                    value = data[i]->value;
                    return (true);
                }
            }
            return (false);
        }

        void add_value(const char* key, float initial_value = 0.0f) {
            data.push_back(new SensorValue(key, initial_value));
        }

        void setup(const SensorProperty* property_list, size_t length) {
            for (size_t i = 0; i < length; i++) {
                add_value(property_list[i].key, property_list[i].bottom);
            }
        }
};

class SensorDriver {
    protected:
        bool          initialized = false;
        bool          error = false;
        bool          suspended = false;
        bool          running = false;
        uint8_t       device_address = INVALID_DEVICE_ADDRESS;
        uint32_t      head_serial_number = 0;
        TaskHandle_t  task_handle = nullptr;
        SwI2CBus      swI2CPort;
        SensorReading reading;
        TickType_t    loop_time_ms = SENSOR_STD_READING_INTERVAL_MS;
        
        static void _driverTask(void* pvParameters);
        void driverTask(void);

    public:
        SensorDriver() {
        }

        virtual ~SensorDriver() {
        }

        virtual esp_err_t enable(void) = 0;
        virtual esp_err_t disable(void) = 0;
        virtual esp_err_t fetch(void) = 0;
        virtual esp_err_t update(void) = 0;

        virtual const SensorProperty* get_properties(void) = 0;
        virtual size_t get_property_count(void) = 0;
        virtual esp_err_t set_heating(bool enable) = 0;
        virtual const char* get_head(void) = 0;
        virtual SensorType get_sensor_type(void) = 0;
        virtual uint8_t get_sub_type(void) = 0;

        SensorReading* get_reading(void);
        void send_measuring_event(void);
        void apply_magnus_formula(float temperature_C, float humidity_RH, float& dewpoint_C, float& humidity_AH);
        bool wait_ready(uint32_t timeout_ms = 100);
        void set_loop_time(TickType_t time_ms);
        size_t search(size_t length, const char* result[], uint8_t* adr_list, SensorType* typeList);
        bool suspend(void);
        SensorType identify_at_adr_0x40(SwI2CBus& bus);
        uint32_t get_head_serial_number(void);

        static SensorDriver* create_driver_by_address(uint8_t bus_addr);
        static uint8_t get_bus_addr_by_type(SensorType type);
        static SensorDriver* auto_scan(SensorType selected = SensorType::autoscan);
};
