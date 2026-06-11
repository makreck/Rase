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

#define AHTXX_LOOP_TIME_MS            (2000)

#define ATHXX_TYPE_UNDEF              (0)
#define ATHXX_TYPE_ATH10              (1)
#define ATHXX_TYPE_ATH21              (2)

// Delays in milliseconds
#define AHTXX_CMD_DELAY               (10)  // delay between commands, in milliseconds
#define AHTXX_MEASUREMENT_DELAY       (250) // wait for measurement to complete, in milliseconds
#define AHT1X_POWER_ON_DELAY          (40)  // wait for AHT1x to initialize after power-on, in milliseconds
#define AHT2X_POWER_ON_DELAY          (100) // wait for AHT2x to initialize after power-on, in milliseconds
#define AHTXX_SOFT_RESET_DELAY        (20)  // less than 20 milliseconds
#define AHTXX_REG_RESET_DELAY         (5)

#define ATHXX_CALIBRATION_TIMEOUT_MS  (3000)
#define ATHXX_MEASURING_TIMEOUT_MS    (250)

// Registers
#define AHT1X_INIT_REG                (0xE1) // initialization register, for AHT1x only
#define AHT2X_INIT_REG                (0xBE) // initialization register, for AHT2x only
#define AHTXX_STATUS_REG              (0x71) // read status byte register

// Commands
#define AHTXX_START_MEASUREMENT_REG   (0xAC) // start measurement register
#define AHTXX_SOFT_RESET_REG          (0xBA) // soft reset register
#define AHTXX_CMD_CALIBRATE           (0xE1) // calibration command

// Calibration register controls
#define AHT1X_INIT_CTRL_NORMAL_MODE   (0x00) // normal mode on/off    bit[6:5], for AHT1x only
#define AHT1X_INIT_CTRL_CYCLE_MODE    (0x20) // cycle mode on/off     bit[6:5], for AHT1x only
#define AHT1X_INIT_CTRL_CMD_MODE      (0x40) // command mode n/off    bit[6:5], for AHT1x only
#define AHTXX_INIT_CTRL_CAL_ON        (0x08) // calibration coeff on/off bit[3]
#define AHTXX_INIT_CTRL_NOP           (0x00) // NOP control, send after any "AHT1X_INIT_CTRL..."

// Status byte register controls
#define AHTXX_STATUS_CTRL_BUSY        (0x80)  // busy                      bit[7]
#define AHT1X_STATUS_CTRL_NORMAL_MODE (0x00)  // normal mode status        bit[6:5], for AHT1x only
#define AHT1X_STATUS_CTRL_CYCLE_MODE  (0x20)  // cycle mode status         bit[6:5], for AHT1x only
#define AHT1X_STATUS_CTRL_CMD_MODE    (0x40)  // command mode status       bit[6:5], for AHT1x only
#define AHTXX_STATUS_CTRL_CRC         (0x10)  // CRC8 status               bit[4], no info in datasheet
#define AHTXX_STATUS_CTRL_CAL_ON      (0x08)  // calibration coeff status  bit[3]
#define AHTXX_STATUS_CTRL_FIFO_ON     (0x04)  // FIFO on status            bit[2], no info in datasheet
#define AHTXX_STATUS_CTRL_FIFO_STATE  (0x02)  // FIFO status               bit[1], no info in datasheet
#define AHTXX_STATUS_CTRL_RES0        (0x01)  // reserved

#define AHTXX_STATUS_OK (AHTXX_STATUS_CTRL_CAL_ON | AHTXX_STATUS_CTRL_CRC)

// Measurement register controls
#define AHTXX_START_MEASUREMENT_CTRL     (0x33) // 0x33? measurement controls, suspect this is temperature & humidity DAC resolution
#define AHTXX_START_MEASUREMENT_CTRL_NOP (0x00) // NOP control, send after any "AHTXX_START_MEASUREMENT_CTRL..."

class SensorAHTxx : public SensorDriver {
    private:
        static const SensorProperty athxx_properties[];

        const char* headName = AHT_DEVICE_NAME;
        uint8_t typeID       = 0x00;

        uint32_t rawDataTemp = 0;
        uint32_t rawDataRH   = 0;
        float temperature_C  = 0.0f;
        float humidity_RH    = 0.0f;
        float dewpoint_C     = 0.0f;
        float humidity_AH    = 0.0f;

        void init(void);
        void cleanup(void);

        esp_err_t reset(void);
        static uint8_t calcCRC8(uint8_t* message, size_t length);

        esp_err_t read_status_register(uint8_t& status);
        esp_err_t soft_reset(void);
        esp_err_t calibration(void);
        esp_err_t set_initialization_register(uint8_t value);
        esp_err_t set_normal_mode(void);
        esp_err_t set_cycle_mode(void);
        esp_err_t set_command_mode(void);
        esp_err_t reset_register(uint8_t regAddr);
        esp_err_t init_registers(void);
        esp_err_t read_serial_number_code(uint32_t& serialNumberCode);
        esp_err_t wait_for_operation(uint32_t timeout_ms);

        bool check_status_bit(uint8_t mask);
        bool is_busy(void);
        bool is_ready(void);

        esp_err_t enable(void) override;
        esp_err_t disable(void) override;
        esp_err_t fetch(void) override;
        esp_err_t update(void) override;
        
    public:
        SensorAHTxx() {
            init();
        }

        ~SensorAHTxx() override {
            cleanup();
        }

        const SensorProperty* get_properties(void) override;
        size_t get_property_count(void) override;
        esp_err_t set_heating(bool enable) override;
        const char* get_head(void) override;
        SensorType get_sensor_type(void) override;
        uint8_t get_sub_type(void) override;
};    
