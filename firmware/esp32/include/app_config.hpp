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

// Note: Defined by "platformio.ini"
//#define SEEED_XIAO_S3
//#define ESP32_S3_ZERO
//#define ESP32_S3_WROOM_1

#ifdef SEEED_XIAO_S3
    #define CHIP_TYPE "Seeed Studio XIAO ESP32-S3"
    #define ESP32_S2S3

    #define _HAS_BOARD_LED
    //#define _HAS_RGB_LED
    //#define _HAS_NEOPIXEL_LED
    #define _BOARD_LED_INVERTED

    #define _ENABLE_LCD
    #define _ENABLE_BUTTON
    //#define _ENABLE_SD_CARD
    //#define _ENABLE_USB_CDC
    //#define _ENABLE_COM1
    //#define _ENABLE_COM2
    #define _ENABLE_WIFI
    //#define _ENABLE_BLUETOOTH
#endif

#ifdef ESP32_S3_ZERO
    #define CHIP_TYPE "Waveshare ESP32-S3 mini"
    #define ESP32_S2S3

    //#define _HAS_BOARD_LED
    //#define _HAS_RGB_LED
    #define _HAS_NEOPIXEL_LED
    //#define _BOARD_LED_INVERTED

    #define _ENABLE_LCD
    #define _ENABLE_BUTTON
    //#define _ENABLE_SD_CARD
    #define _ENABLE_USB_CDC
    //#define _ENABLE_COM1
    //#define _ENABLE_COM2
    #define _ENABLE_WIFI
    //#define _ENABLE_BLUETOOTH
#endif

#ifdef ESP32_S3_WROOM_1
    #define CHIP_TYPE "ESP32-S3 Wroom"
    #define ESP32_S2S3

    //#define _HAS_BOARD_LED
    //#define _HAS_RGB_LED
    #define _HAS_NEOPIXEL_LED
    //#define _BOARD_LED_INVERTED

    #define _ENABLE_LCD
    #define _ENABLE_BUTTON
    //#define _ENABLE_SD_CARD
    #define _ENABLE_USB_CDC
    //#define _ENABLE_COM1
    #define _ENABLE_COM2
    #define _ENABLE_WIFI
    //#define _ENABLE_BLUETOOTH
#endif

#ifdef ESP32_WROOM_DEV
    #define CHIP_TYPE "ESP32 Wroom"
    #define _HAS_BOARD_LED
    //#define _HAS_RGB_LED
    //#define _HAS_NEOPIXEL_LED
    //#define _BOARD_LED_INVERTED

    #define _ENABLE_LCD
    #define _ENABLE_BUTTON
    //#define _ENABLE_SD_CARD
    //#define _ENABLE_USB_CDC
    //#define _ENABLE_COM1
    //#define _ENABLE_COM2
    #define _ENABLE_WIFI
    //#define _ENABLE_BLUETOOTH
#endif

#ifdef ESP32_WROVER_DEV
    #define CHIP_TYPE "ESP32 Wroover"
    #define _HAS_BOARD_LED
    #define _HAS_RGB_LED
    #define _HAS_NEOPIXEL_LED
    //#define _BOARD_LED_INVERTED

    #define _ENABLE_LCD
    #define _ENABLE_BUTTON
    //#define _ENABLE_SD_CARD
    //#define _ENABLE_USB_CDC
    //#define _ENABLE_COM1
    //#define _ENABLE_COM2
    #define _ENABLE_WIFI
    //#define _ENABLE_BLUETOOTH
#endif
