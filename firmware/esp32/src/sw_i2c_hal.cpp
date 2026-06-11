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

#include "app.hpp"

esp_err_t SwI2CBus::enable(void) {
    sclInit();
    sdaInit();
    stop();
    longWait();
    return (ESP_OK);
}


void SwI2CBus::bitWait(void) {
    //vTaskDelay(1);
    //esp_rom_delay_us(500);    
    esp_rom_delay_us(5);    
}

void SwI2CBus::longWait(void) {
    vTaskDelay(3);
}

void SwI2CBus::millisecondWait(TickType_t ms) { 
    vTaskDelay(MAX(1, pdMS_TO_TICKS(ms)));
}

esp_err_t SwI2CBus::start_write_access(uint8_t bAddress) {
    start();
    return (send_byte(((bAddress << 1) | 0)));
}

esp_err_t SwI2CBus::start_read_access(uint8_t bAddress) {
    start();
    return (send_byte(((bAddress << 1) | 1)));
}

esp_err_t SwI2CBus::stop_access(void) {
    return (stop());
}

esp_err_t SwI2CBus::disable(void) {
    sclInput();
    sclHigh();
    sdaInput();
    sdaHigh();
    return (ESP_OK);
}

esp_err_t SwI2CBus::start(void) {
    sclOutput();
    sdaOutput();
    bitWait();
    sdaHigh();
    bitWait();
    sclHigh();
    bitWait();
    sdaLow();
    bitWait();
    sclLow();
    bitWait();
    return (ESP_OK);
}

esp_err_t SwI2CBus::stop(void) {
    sclOutput();
    sdaOutput();
    bitWait();
    sclLow();
    bitWait();
    sdaLow();
    bitWait();
    sclHigh();
    bitWait();
    sdaHigh();
    bitWait();
    return (ESP_OK);
}

esp_err_t SwI2CBus::detect(uint8_t bAddress, int retry) {
    retry = std::max(1, std::min(I2C_MAX_RETRY, retry));
    esp_err_t hr = ESP_FAIL;
    for (int trial = 0; (trial < retry) && (hr != ESP_OK); trial++) {
        enable();
        hr = start_write_access(bAddress);
        stop();
    }
    return (hr);
}

esp_err_t SwI2CBus::send_byte(uint8_t bData) {
    esp_err_t hr = ESP_OK;

    sclOutput();
    sdaOutput();

    for (uint8_t bMask = 0x80; bMask > 0; bMask >>= 1) {
        if (bData & bMask) {
            sdaHigh();
        } else {
            sdaLow();
        }
        bitWait();

        sclHigh();
        bitWait();

        sclLow();
        bitWait();
    }

    sdaInput();
    sclHigh();
    bitWait();
    hr = getAckNack();
    sclLow();

    longWait();

    return (hr);
}

esp_err_t SwI2CBus::receive_byte(uint8_t *pbData, uint8_t bAckState) {
    uint8_t bData = 0x00;

    sclOutput();
    sdaInput();
    sdaHigh();

    for (uint8_t bMask = 0x80; bMask > 0; bMask >>= 1) {
        sclHigh();
        bitWait();
        if (sdaRead()) {
            bData |= bMask;
        }
        sclLow();
        bitWait();
    }

    sdaOutput();
    if (bAckState == 0) {
        sdaLow();
    } else {
        sdaHigh();
    }
    sclHigh();

    bitWait();
    sclLow();

    bitWait();
    sdaHigh();

    *pbData = bData;

    return (ESP_OK);
}

esp_err_t SwI2CBus::burst_read(const uint8_t address, uint8_t* buffer, const size_t length) {
    esp_err_t result = ESP_OK;
    if ((length > 0) && (buffer != nullptr)) {
        result = start_read_access(address);
        for (size_t i = 0; (i < length) && (result == ESP_OK); i++) {
            result = receive_byte(&buffer[i], (i == (length - 1)) ? 1 : 0);
        }
    }
    return (result);
}

esp_err_t SwI2CBus::burst_write(const uint8_t address, const uint8_t* buffer, const size_t length) {
    esp_err_t result = ESP_OK;
    if ((length > 0) && (buffer != nullptr)) {
        result = start_write_access(address);
        for (size_t i = 0; (i < length) && (result == ESP_OK); i++) {
            result = send_byte(buffer[i]);
        }
    }
    return (result);
}

esp_err_t SwI2CBus::send_command(const uint8_t address, uint8_t cmd) {
    esp_err_t result = start_write_access(address);
    if (result == ESP_OK) {
        result = send_byte(cmd);
    }
    stop();
    return (result);
}

esp_err_t SwI2CBus::write_register(const uint8_t address, uint8_t regAddr, uint8_t dataByte) {
    esp_err_t result = start_write_access(address);
    if (result == ESP_OK) {
        result = send_byte(regAddr);
        if (result == ESP_OK) {
            result = send_byte(dataByte);
        }
    }
    stop();
    return (result);
}

esp_err_t SwI2CBus::read_register(const uint8_t address, uint8_t regAddr, uint8_t& dataByte) {
    dataByte = 0xff;
    esp_err_t result = start_write_access(address);
    if (result == ESP_OK) {
        result = send_byte(regAddr);
        if (result == ESP_OK) {
            result = start_read_access(address);
            if (result == ESP_OK) {
                result = receive_byte(&dataByte, 1);
            }
        }
    }
    stop();
    return (result);
}

esp_err_t SwI2CBus::send_command_sequence(const uint8_t address, const uint8_t* cmds, size_t length) {
    esp_err_t result = burst_write(address, cmds, length);
    stop();
    return (result);
}

esp_err_t SwI2CBus::transact(const uint8_t address, const uint8_t* outData, const size_t outLength, uint8_t* inData, const size_t inLength, uint32_t wait_ms, bool stopAfterWrite) {
    esp_err_t result = burst_write(address, outData, outLength);

    if (stopAfterWrite) {
        stop();
    }

    if (result == ESP_OK) {
        if (wait_ms != 0) {
            millisecondWait(wait_ms);
            //vTaskDelay(MAX(1, pdMS_TO_TICKS(wait_ms)));
        }

        result = burst_read(address, inData, inLength);
    }

    stop();
    return (result);
}

esp_err_t SwI2CBus::transact8(const uint8_t address, const uint8_t command8, uint8_t* inData, const size_t inLength, uint32_t wait_ms, bool stopAfterWrite) {
    uint8_t outData[1] { command8 };
    return (transact(address, outData, SIZEOFARRAY(outData), inData, inLength, wait_ms, stopAfterWrite));
}

esp_err_t SwI2CBus::transact16(const uint8_t address, const uint16_t command16, uint8_t* inData, const size_t inLength, uint32_t wait_ms, bool stopAfterWrite) {
    uint8_t outData[2] { MSB(command16), LSB(command16) };
    return (transact(address, outData, SIZEOFARRAY(outData), inData, inLength, wait_ms, stopAfterWrite));
}
