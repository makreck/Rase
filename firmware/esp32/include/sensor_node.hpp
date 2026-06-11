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

typedef const char* SensorNode_t;
#define RK_PZ "percent"
#define RK_T  "temperature"
#define RK_RH "rel_humidity"
#define RK_AH "abs_humidity"
#define RK_DP "dewpoint"
#define RK_AP "air_pressure"

class SensorNodeProps {
    public:
        char    name[16]{ 0 };
        char    shortcut[4]{ 0 };
        char    unit[8]{ 0 };
        float   bottom  = 0.0f;
        float   top     = 1.0f;
        float   value   = 0.0f;
        float   min     = 0.0f;
        float   max     = 0.0f;
        float   average = 0.0f;
        int32_t count   = 0.0f;
};

class SensorNode {
    private:
        static const char* json_format;
        static const char* opcua_json_format;

        struct {
            const    char* key      = nullptr;
            const    char* name     = nullptr;
            const    char* shortcut = nullptr;
            const    char* unit     = nullptr;
            const    char* color    = nullptr;
            float    bottom         = 0.0f;
            float    top            = 1.0f;
            float    value          = 0.0f;
            float    min            = 0.0f;
            float    max            = 0.0f;
            float    sum            = 0.0f;
            uint32_t flags          = 0;
            int32_t  count          = 0;
            
            ChannelHistory history;

            char* json = nullptr;
            size_t length = 0;

            int node_id = 0;
            char* opcua_json = nullptr;
            size_t opcua_length = 0;

            SemaphoreHandle_t mutex = nullptr;
        } m;

        void init(const char *_key, const char *_name, const char *_shortcut, const char *_unit, const char* _color, float _bottom, float _top, uint32_t _flags, int _node_id);
        void clear(void);
        void update(void);
        void update_opcua(void);

    public:
        SensorNode(const SensorProperty* properties);
        ~SensorNode();

        const char* get_key(void) { return (m.key); }

        const char* get_json(void);
        size_t get_length(void);

        const char* get_opcua_json(void);
        size_t get_opcua_length(void);

        void get_properties(SensorNodeProps& props, bool modify_unit = true);

        void reset(void);
        AppState add_Measurement(float value);
};
