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

class SensorNull : public SensorDriver {
    private:
        static constexpr const char* headName = "Null";
        static const SensorProperty null_properties[];

        float sim_value[6]{ 0.5f };
        
        esp_err_t init(void);
        esp_err_t cleanup(void);
        
        esp_err_t enable(void) override;
        esp_err_t disable(void) override;
        esp_err_t fetch(void) override;
        esp_err_t update(void) override;

    public:
        SensorNull() {
            init();
        }

        ~SensorNull() override {
            cleanup();
        }

        const SensorProperty* get_properties(void) override;
        size_t get_property_count(void) override;
        esp_err_t set_heating(bool enable) override;
        const char* get_head(void) override;
        SensorType get_sensor_type(void) override;
        uint8_t get_sub_type(void) override;
};    
