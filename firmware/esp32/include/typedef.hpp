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

#define SIZEOFARRAY(Array) (sizeof(Array) / sizeof((Array)[0]))

#define STR2(a) #a
#define STR(a)  STR2(a)

#define MSB(x) (uint8_t)((((uint16_t)(x)) >> 8) & 0x00ff)
#define LSB(x) (uint8_t)((((uint16_t)(x)) >> 0) & 0x00ff)
#define BTOW(x) (uint16_t)(((uint16_t)(x)[0] << 8) | ((uint16_t)(x)[1] << 0))
#define BTOL(x) (uint32_t)(((uint32_t)(x)[0] << 24) | ((uint32_t)(x)[1] << 16) | ((uint32_t)(x)[2] << 8) | ((uint32_t)(x)[3] << 0))
#define B3TOL(x) (uint32_t)(((uint32_t)(x)[0] << 24) | ((uint32_t)(x)[1] << 16) | ((uint32_t)(x)[3] << 8) | ((uint32_t)(x)[4] << 0))

typedef union _TypeConvert {
    uint8_t b[4];
    uint16_t w[2];
    uint32_t dw;
} TypeConvert;

#define SAFE_DELETE(x)  \
    if (x != nullptr) { \
        delete (x);     \
        x = nullptr;    \
    }

#define TASK_DEFAULT_PRIORITY   (20)
#define TASK_SMALL_STACKSIZE    (1024)
#define TASK_DEFAULT_STACKSIZE  (4096)
#define TASK_EXTENDED_STACKSIZE (8192)

#define TIME_STAMP_LENGTH       (24)
#define TIME_EMPTY_TIME_STRING  "0000-00-00T00:00:00.000"
#define TIME_ZONE_DEFAULT       "CET-1CEST,M3.5.0,M10.5.0/3"
