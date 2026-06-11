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

#ifdef SEEED_XIAO_S3

#define ESP32_BOOT_BUTTON       (GPIO_NUM_1)
#define ESP32_ALT_BUTTON1       (GPIO_NUM_NC)
#define ESP32_ALT_BUTTON2       (GPIO_NUM_NC)
#define BOOTLOADER_TXD_UART0    (GPIO_NUM_43)
#define BOOTLOADER_RXD_UART0    (GPIO_NUM_44)

#define SD_CARD_D0_MISO         (GPIO_NUM_NC)
#define SD_CARD_D1_IRQ          (GPIO_NUM_NC)
#define SD_CARD_D2_NC           (GPIO_NUM_NC)
#define SD_CARD_D3_CS           (GPIO_NUM_NC)
#define SD_CARD_CLK_SCLK        (GPIO_NUM_NC)
#define SD_CARD_CMD_MOSI        (GPIO_NUM_NC)

#define SENSOR_DAC_0            (GPIO_NUM_NC)
#define SENSOR_DAC_1            (GPIO_NUM_NC)

#define LED_DEVBOARD_PIN        (GPIO_NUM_21)
#define RGBLED_PIN_R            (GPIO_NUM_NC)
#define RGBLED_PIN_G            (GPIO_NUM_NC)
#define RGBLED_PIN_B            (GPIO_NUM_NC)
#define LED_NEOPIXEL            (GPIO_NUM_7)

#define SENSOR_PORT_SDA         (GPIO_NUM_5)
#define SENSOR_PORT_SCL         (GPIO_NUM_44)

#define DISPLAY_I2C_SDA         (GPIO_NUM_8)
#define DISPLAY_I2C_SCL         (GPIO_NUM_9)

#define SENSOR_ADC1_0           (GPIO_NUM_1)
#define SENSOR_ADC1_1           (GPIO_NUM_2)
#define SENSOR_ADC1_2           (GPIO_NUM_3)
#define SENSOR_ADC1_3           (GPIO_NUM_4)

#endif


#ifdef ESP32_S3_WROOM_1

#define ESP32_BOOT_BUTTON       (GPIO_NUM_18)
#define ESP32_ALT_BUTTON1       (GPIO_NUM_NC)
#define ESP32_ALT_BUTTON2       (GPIO_NUM_NC)
#define BOOTLOADER_TXD_UART0    (GPIO_NUM_43)
#define BOOTLOADER_RXD_UART0    (GPIO_NUM_44)

#define SD_CARD_D0_MISO         (GPIO_NUM_NC)
#define SD_CARD_D1_IRQ          (GPIO_NUM_NC)
#define SD_CARD_D2_NC           (GPIO_NUM_NC)
#define SD_CARD_D3_CS           (GPIO_NUM_NC)
#define SD_CARD_CLK_SCLK        (GPIO_NUM_NC)
#define SD_CARD_CMD_MOSI        (GPIO_NUM_NC)

#define UDB_D_MINUS             (GPIO_NUM_19)
#define USB_D_PLUS              (GPIO_NUM_20)

#define SENSOR_PORT_SDA         (GPIO_NUM_21)
#define SENSOR_PORT_SCL         (GPIO_NUM_47)

#define SENSOR_DAC_0            (GPIO_NUM_1)
#define SENSOR_DAC_1            (GPIO_NUM_2)

#define LED_DEVBOARD_PIN        (GPIO_NUM_NC)
#define RGBLED_PIN_R            (GPIO_NUM_NC)
#define RGBLED_PIN_G            (GPIO_NUM_NC)
#define RGBLED_PIN_B            (GPIO_NUM_NC)
#define LED_NEOPIXEL            (GPIO_NUM_48)

#define DISPLAY_I2C_SDA         (GPIO_NUM_16)
#define DISPLAY_I2C_SCL         (GPIO_NUM_15)

#define SENSOR_ADC1_0           (GPIO_NUM_4)
#define SENSOR_ADC1_1           (GPIO_NUM_5)
#define SENSOR_ADC1_2           (GPIO_NUM_6)
#define SENSOR_ADC1_3           (GPIO_NUM_7)

#endif


#ifdef ESP32_S3_ZERO

#define ESP32_BOOT_BUTTON       (GPIO_NUM_0)
#define ESP32_ALT_BUTTON1       (GPIO_NUM_6)
#define ESP32_ALT_BUTTON2       (GPIO_NUM_5)
#define BOOTLOADER_TXD_UART0    (GPIO_NUM_43)
#define BOOTLOADER_RXD_UART0    (GPIO_NUM_44)

#define SD_CARD_D0_MISO         (GPIO_NUM_NC)
#define SD_CARD_D1_IRQ          (GPIO_NUM_NC)
#define SD_CARD_D2_NC           (GPIO_NUM_NC)
#define SD_CARD_D3_CS           (GPIO_NUM_NC)
#define SD_CARD_CLK_SCLK        (GPIO_NUM_NC)
#define SD_CARD_CMD_MOSI        (GPIO_NUM_NC)

#define SENSOR_DAC_0            (GPIO_NUM_NC)
#define SENSOR_DAC_1            (GPIO_NUM_NC)

#define LED_DEVBOARD_PIN        (GPIO_NUM_NC)
#define RGBLED_PIN_R            (GPIO_NUM_NC)
#define RGBLED_PIN_G            (GPIO_NUM_NC)
#define RGBLED_PIN_B            (GPIO_NUM_NC)
#define LED_NEOPIXEL            (GPIO_NUM_21)

#define SENSOR_PORT_SDA         (GPIO_NUM_7)
#define SENSOR_PORT_SCL         (GPIO_NUM_8)

#define DISPLAY_I2C_SDA         (GPIO_NUM_2)
#define DISPLAY_I2C_SCL         (GPIO_NUM_1)

#define SENSOR_ADC1_0           (GPIO_NUM_NC)
#define SENSOR_ADC1_1           (GPIO_NUM_NC)
#define SENSOR_ADC1_2           (GPIO_NUM_NC)
#define SENSOR_ADC1_3           (GPIO_NUM_NC)

#endif

#ifdef ESP32_WROOM_DEV

#define ESP32_BOOT_BUTTON       (GPIO_NUM_0)
#define ESP32_ALT_BUTTON1       (GPIO_NUM_NC)
#define ESP32_ALT_BUTTON2       (GPIO_NUM_NC)
#define BOOTLOADER_TXD_UART0    (GPIO_NUM_1)
#define BOOTLOADER_RXD_UART0    (GPIO_NUM_3)

#define SD_CARD_CLK_SCLK        (GPIO_NUM_6)
#define SD_CARD_D0_MISO         (GPIO_NUM_7)
#define SD_CARD_D1_IRQ          (GPIO_NUM_8)
#define SD_CARD_D2_NC           (GPIO_NUM_9)
#define SD_CARD_D3_CS           (GPIO_NUM_10)
#define SD_CARD_CMD_MOSI        (GPIO_NUM_11)

#define COM1_RXD                (BOOTLOADER_RXD_UART0)
#define COM1_TXD                (BOOTLOADER_TXD_UART0)

#define COM2_RXD                (GPIO_NUM_18)
#define COM2_TXD                (GPIO_NUM_19)

#define SENSOR_PORT_SDA         (GPIO_NUM_21)
#define SENSOR_PORT_SCL         (GPIO_NUM_22)
#define DISPLAY_I2C_SDA         (GPIO_NUM_32)
#define DISPLAY_I2C_SCL         (GPIO_NUM_33)

#define LED_DEVBOARD_PIN        (GPIO_NUM_2)
#define RGBLED_PIN_R            (GPIO_NUM_NC)
#define RGBLED_PIN_G            (GPIO_NUM_NC)
#define RGBLED_PIN_B            (GPIO_NUM_NC)
#define LED_NEOPIXEL            (GPIO_NUM_13)

#define SENSOR_ADC1_0           (GPIO_NUM_34)
#define SENSOR_ADC1_1           (GPIO_NUM_35)
#define SENSOR_ADC1_2           (GPIO_NUM_36)
#define SENSOR_ADC1_3           (GPIO_NUM_39)

#endif

#ifdef ESP32_WROVER_DEV

#define ESP32_BOOT_BUTTON       (GPIO_NUM_0)
#define ESP32_ALT_BUTTON1       (GPIO_NUM_NC)
#define ESP32_ALT_BUTTON2       (GPIO_NUM_NC)
#define BOOTLOADER_TXD_UART0    (GPIO_NUM_1)
#define BOOTLOADER_RXD_UART0    (GPIO_NUM_3)

#define SD_CARD_CLK_SCLK        (GPIO_NUM_6)
#define SD_CARD_D0_MISO         (GPIO_NUM_7)
#define SD_CARD_D1_IRQ          (GPIO_NUM_8)
#define SD_CARD_D2_NC           (GPIO_NUM_9)
#define SD_CARD_D3_CS           (GPIO_NUM_10)
#define SD_CARD_CMD_MOSI        (GPIO_NUM_11)

#define COM1_RXD                (BOOTLOADER_RXD_UART0)
#define COM1_TXD                (BOOTLOADER_TXD_UART0)

#define COM2_RXD                (GPIO_NUM_18)
#define COM2_TXD                (GPIO_NUM_19)

#define SENSOR_PORT_SDA         (GPIO_NUM_21)
#define SENSOR_PORT_SCL         (GPIO_NUM_22)
#define DISPLAY_I2C_SDA         (GPIO_NUM_32)
#define DISPLAY_I2C_SCL         (GPIO_NUM_33)

#define SENSOR_DAC_0            (GPIO_NUM_25)
#define SENSOR_DAC_1            (GPIO_NUM_26)

#define LED_DEVBOARD_PIN        (GPIO_NUM_2)
#define RGBLED_PIN_R            (GPIO_NUM_5)
#define RGBLED_PIN_G            (GPIO_NUM_23)
#define RGBLED_PIN_B            (GPIO_NUM_27)
#define LED_NEOPIXEL            (GPIO_NUM_13)

#define SENSOR_ADC1_0           (GPIO_NUM_34)
#define SENSOR_ADC1_1           (GPIO_NUM_35)
#define SENSOR_ADC1_2           (GPIO_NUM_36)
#define SENSOR_ADC1_3           (GPIO_NUM_39)

#endif
