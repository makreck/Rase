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

#define SHT_POWER_ON_DELAY_MS     (200)
#define SHT_CLEAR_STATUS_DELAY_MS (15)

class STH3State {
    public:
        union {
            uint16_t state;
            struct {
                uint16_t checkSumState : 1; // checksum state of the last transfer
                uint16_t cmdState : 1;      // last command state
                uint16_t reserved2_3 : 2;   // 2 reserved bits
                uint16_t resetDetect : 1;   // reset detected since last status register clear
                uint16_t reserved5_9 : 5;   // 5 reserved bits
                uint16_t tempAlert : 1;     // temperature tracking alert
                uint16_t rhAlert : 1;       // humidity tracking alert
                uint16_t reserved12 : 1;    // 1 reserved bit
                uint16_t heaterEnable : 1;  // heater enable flag
                uint16_t reserved14 : 1;    // 1 reserved bit
                uint16_t alertPending : 1;  // alert is pending flag
            };
        };

        STH3State(void) {
            clear();
        }

        ~STH3State() {
        }

        void clear(void) {
            state = 0;
        }

        uint16_t get(void) {
            return (state);
        }

        void set(uint8_t* ioBytes) {
            state = ((uint16_t)ioBytes[0] << 8) | ((uint16_t)ioBytes[1]);
        }
};

#define SHT3_STAT_CHKSUM               (0x0001) // last checksum state
#define SHT3_STAT_CMD                  (0x0002) // last command state
#define SHT3_STAT_RES2                 (0x0004) // reserved bit #2
#define SHT3_STAT_RES3                 (0x0008) // reserved bit #3
#define SHT3_STAT_RESET_DETECT         (0x0010) // reset detected since last clear status
#define SHT3_STAT_RES5                 (0x0020) // reserved bit #5
#define SHT3_STAT_RES6                 (0x0040) // reserved bit #6
#define SHT3_STAT_RES7                 (0x0080) // reserved bit #7
#define SHT3_STAT_RES8                 (0x0100) // reserved bit #8
#define SHT3_STAT_RES9                 (0x0200) // reserved bit #9
#define SHT3_STAT_TEMP_ALERT           (0x0400) // temperature tracking alert flag
#define SHT3_STAT_RH_ALERT             (0x0800) // humidity tracking alert flag
#define SHT3_STAT_RES12                (0x1000) // reserved bit #12
#define SHT3_STAT_HEATER_ENABLE        (0x2000) // heater enable
#define SHT3_STAT_RES14                (0x4000) // reserved bit #14
#define SHT3_STAT_ALERT_PENDING        (0x8000) // at least one alert is pending flag

// CRC polynominal, P(x) = x^8 + x^5 + x^4 + 1 = 0000000100110001 = 0x0131
#define I2C_POLYNOMIAL                 (0x0131) // CRC polynominal

enum class SHT3Command : uint16_t {
  SHT3_CMD_READ_SERIALNBR        = 0x3780,      // read serial number
  SHT3_CMD_READ_STATUS           = 0xF32D,      // read status register
  SHT3_CMD_CLEAR_STATUS          = 0x3041,      // clear status register
  SHT3_CMD_HEATER_ENABLE         = 0x306D,      // enabled heater
  SHT3_CMD_HEATER_DISABLE        = 0x3066,      // disable heater
  SHT3_CMD_SOFT_RESET            = 0x30A2,      // soft reset
  SHT3_CMD_MEAS_CLOCKSTR_H       = 0x2C06,      // measurement: clock stretching, high repeatability
  SHT3_CMD_MEAS_CLOCKSTR_M       = 0x2C0D,      // measurement: clock stretching, medium repeatability
  SHT3_CMD_MEAS_CLOCKSTR_L       = 0x2C10,      // measurement: clock stretching, low repeatability
  SHT3_CMD_MEAS_POLLING_H        = 0x2400,      // measurement: polling, high repeatability
  SHT3_CMD_MEAS_POLLING_M        = 0x240B,      // measurement: polling, medium repeatability
  SHT3_CMD_MEAS_POLLING_L        = 0x2416,      // measurement: polling, low repeatability
  SHT3_CMD_MEAS_PERI_05_H        = 0x2032,      // measurement: periodic 0.5 mps, high repeatability
  SHT3_CMD_MEAS_PERI_05_M        = 0x2024,      // measurement: periodic 0.5 mps, medium repeatability
  SHT3_CMD_MEAS_PERI_05_L        = 0x202F,      // measurement: periodic 0.5 mps, low repeatability
  SHT3_CMD_MEAS_PERI_1_H         = 0x2130,      // measurement: periodic 1 mps, high repeatability
  SHT3_CMD_MEAS_PERI_1_M         = 0x2126,      // measurement: periodic 1 mps, medium repeatability
  SHT3_CMD_MEAS_PERI_1_L         = 0x212D,      // measurement: periodic 1 mps, low repeatability
  SHT3_CMD_MEAS_PERI_2_H         = 0x2236,      // measurement: periodic 2 mps, high repeatability
  SHT3_CMD_MEAS_PERI_2_M         = 0x2220,      // measurement: periodic 2 mps, medium repeatability
  SHT3_CMD_MEAS_PERI_2_L         = 0x222B,      // measurement: periodic 2 mps, low repeatability
  SHT3_CMD_MEAS_PERI_4_H         = 0x2334,      // measurement: periodic 4 mps, high repeatability
  SHT3_CMD_MEAS_PERI_4_M         = 0x2322,      // measurement: periodic 4 mps, medium repeatability
  SHT3_CMD_MEAS_PERI_4_L         = 0x2329,      // measurement: periodic 4 mps, low repeatability
  SHT3_CMD_MEAS_PERI_10_H        = 0x2737,      // measurement: periodic 10 mps, high repeatability
  SHT3_CMD_MEAS_PERI_10_M        = 0x2721,      // measurement: periodic 10 mps, medium repeatability
  SHT3_CMD_MEAS_PERI_10_L        = 0x272A,      // measurement: periodic 10 mps, low repeatability
  SHT3_CMD_MEAS_ART              = 0x2B32,      // measurement: periodic 4 mps, accelerated response time
  SHT3_CMD_FETCH_DATA            = 0xE000,      // readout measurements for periodic mode
  SHT3_CMD_R_AL_LIM_LS           = 0xE102,      // read alert limits, low set
  SHT3_CMD_R_AL_LIM_LC           = 0xE109,      // read alert limits, low clear
  SHT3_CMD_R_AL_LIM_HS           = 0xE11F,      // read alert limits, high set
  SHT3_CMD_R_AL_LIM_HC           = 0xE114,      // read alert limits, high clear
  SHT3_CMD_W_AL_LIM_HS           = 0x611D,      // write alert limits, high set
  SHT3_CMD_W_AL_LIM_HC           = 0x6116,      // write alert limits, high clear
  SHT3_CMD_W_AL_LIM_LC           = 0x610B,      // write alert limits, low clear
  SHT3_CMD_W_AL_LIM_LS           = 0x6100,      // write alert limits, low set
  SHT3_CMD_NO_SLEEP              = 0x303E,      // No sleep mode
};

class SensorSHT3 : public SensorDriver {
    private:
        static const SensorProperty sht3_properties[];

        const char* headName   = SHT3_DEVICE_NAME;
        uint16_t wRawDataTemp  = 0;
        uint16_t wRawDataRH    = 0;
        float temperature_C    = 0.0f;
        float pressure_hPa     = 0.0f;
        float humidity_RH      = 0.0f;
        float dewpoint_C       = 0.0f;
        float humidity_AH      = 0.0f;

        STH3State status;

        static uint8_t calc_crc(uint8_t* pData, uint8_t nLen);
        static bool bi_value_crc_check(uint8_t* buffer);
        static float convert_raw_data_RH(uint16_t wRawDataRH);
        static float convert_raw_data_Temp(uint16_t wRawDataTemp);

        esp_err_t init(void);
        esp_err_t cleanup(void);

        esp_err_t read_serial_number_code(uint32_t& serialNoCode);
        esp_err_t read_status_code(STH3State& pStatusCode);
        esp_err_t clear_status_code(void);
        esp_err_t start_periodic_measurement(SHT3Command CmdCode);
        esp_err_t enable_ART(void);
        esp_err_t set_heating(SHT3Command Cmd);

        esp_err_t enable(void) override;
        esp_err_t disable(void) override;
        esp_err_t fetch(void) override;
        esp_err_t update(void) override;

    public:
        SensorSHT3() {
            init();
        }

        ~SensorSHT3() override {
            cleanup();
        }

        const SensorProperty* get_properties(void) override;
        size_t get_property_count(void) override;
        esp_err_t set_heating(bool enable) override;
        const char* get_head(void) override;
        SensorType get_sensor_type(void) override;
        uint8_t get_sub_type(void) override;
};
