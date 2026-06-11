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

#define S_ACK   (ESP_OK)
#define S_NAK   (ESP_FAIL)

#define I2C_ACK (0)
#define I2C_NAK (1)

#define I2C_MAX_RETRY (5)

class SwI2CBus {
    private:
        gpio_num_t sda  = SENSOR_PORT_SDA;
        gpio_num_t scl  = SENSOR_PORT_SCL;

        void sclInit()   { gpio_reset_pin(scl); gpio_set_pull_mode(scl, GPIO_PULLUP_ONLY); gpio_set_level(scl, 1); gpio_set_direction(scl, GPIO_MODE_OUTPUT); }
        void sclLow()    { gpio_set_level(scl, 0); }
        void sclHigh()   { gpio_set_level(scl, 1); }
        void sclInput()  { gpio_set_direction(scl, GPIO_MODE_INPUT); }
        void sclOutput() { gpio_set_direction(scl, GPIO_MODE_OUTPUT); }
        int  sclRead()   { return (gpio_get_level(scl)); }

        void sdaInit()   { gpio_reset_pin(sda); gpio_set_pull_mode(sda, GPIO_PULLUP_ONLY); gpio_set_level(sda, 1); gpio_set_direction(sda, GPIO_MODE_INPUT); }
        void sdaLow()    { gpio_set_level(sda, 0); }
        void sdaHigh()   { gpio_set_level(sda, 1); }
        void sdaInput()  { gpio_set_direction(sda, GPIO_MODE_INPUT); }
        void sdaOutput() { gpio_set_direction(sda, GPIO_MODE_OUTPUT); }
        int  sdaRead()   { return (gpio_get_level(sda)); }

        esp_err_t getAckNack() { return ((sdaRead() == 1) ? S_NAK : S_ACK); }

        void millisecondWait(TickType_t ms);
        void bitWait(void);
        void longWait(void);
        
    public:
        SwI2CBus(gpio_num_t _sda = SENSOR_PORT_SDA, gpio_num_t _scl = SENSOR_PORT_SCL) {
            sda = _sda;
            scl = _scl;
        }

        ~SwI2CBus() {
            disable();
        }
        
        esp_err_t enable(void);
        esp_err_t disable(void);
        esp_err_t start(void);
        esp_err_t stop(void);
        esp_err_t detect(uint8_t bAddress, int retry = 1);
        esp_err_t send_byte(uint8_t bData);
        esp_err_t receive_byte(uint8_t* pbData, uint8_t bAckState);
        esp_err_t start_write_access(uint8_t bAddress);
        esp_err_t start_read_access(uint8_t bAddress);
        esp_err_t stop_access(void);
        esp_err_t burst_write(const uint8_t address, const uint8_t* buffer, const size_t length);
        esp_err_t burst_read(const uint8_t address, uint8_t* buffer, const size_t length);

        esp_err_t send_command(const uint8_t address, uint8_t cmd);
        esp_err_t write_register(const uint8_t address, uint8_t regAddr, uint8_t dataByte);
        esp_err_t read_register(const uint8_t address, uint8_t regAddr, uint8_t& dataByte);
        esp_err_t send_command_sequence(const uint8_t address, const uint8_t* cmds, size_t length);
        esp_err_t transact(const uint8_t address, const uint8_t* outData, const size_t outLength, uint8_t* inData, const size_t inLength, uint32_t wait_ms = 0, bool stopAfterWrite = false);
        esp_err_t transact8(const uint8_t address, const uint8_t command8, uint8_t* inData, const size_t inLength, uint32_t wait_ms = 0, bool stopAfterWrite = false);
        esp_err_t transact16(const uint8_t address, const uint16_t command16, uint8_t* inData, const size_t inLength, uint32_t wait_ms = 0, bool stopAfterWrite = false);
};
