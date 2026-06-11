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

#pragma once

#define VS_HIGH                (0)
#define VS_LOW                 (0)
#define VS_REV                 (0)
#define VS_BUILD               (1)

#define VERSION_BUILD          (((VS_HIGH  << 24) & 0xff000000) | \
                                ((VS_LOW   << 16) & 0x00ff0000) | \
                                ((VS_REV   <<  8) & 0x0000ff00) | \
                                ((VS_BUILD <<  0) & 0x000000ff))

#define VERSION_BUILD_SHORT    (((VS_HIGH  << 12) & 0xf000) | \
                                ((VS_LOW   <<  8) & 0x0f00) | \
                                ((VS_REV   <<  4) & 0x00f0) | \
                                ((VS_BUILD <<  0) & 0x000f))

#define VERSION_BUILD_DATE     (__DATE__)

#define VERSION_NAME           "rase" 

#define MANUFACTURER_ID        "KKS-Elektronik"
#define PRODUCT_ID             "Radio Sensor"
#define SENSOR_ID              "RASE"
