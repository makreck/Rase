/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,      Preliminary Configuration Tool
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

typedef uint32_t ColorRef;

#define RGBA(_r, _g, _b, _a) (ColorRef)((((int)(_a) & 0xff) << 24) | \
                                        (((int)(_b) & 0xff) << 16) | \
                                        (((int)(_g) & 0xff) <<  8) | \
                                        (((int)(_r) & 0xff) <<  0))

#define C_WHITE       (RGBA(255, 255, 255, 255))
#define C_BLACK       (RGBA(  0,   0,   0, 255))
#define C_TRANSPARENT (RGBA(  0,   0,   0,   0))
#define C_RED         (RGBA(255,   0,   0, 255))
#define C_GREEN       (RGBA(  0, 255,   0, 255))
#define C_BLUE        (RGBA(  0,   0, 255, 255))
#define C_YELLOW      (RGBA(255, 255,   0, 255))
#define C_CYAN        (RGBA(  0, 255, 255, 255))
#define C_MAGENTA     (RGBA(255,   0, 255, 255))

#define RGBA_A(c)     ((uint8_t)(((uint32_t)(c) >> 24) & 0xff))
#define RGBA_B(c)     ((uint8_t)(((uint32_t)(c) >> 16) & 0xff))
#define RGBA_G(c)     ((uint8_t)(((uint32_t)(c) >>  8) & 0xff))
#define RGBA_R(c)     ((uint8_t)(((uint32_t)(c) >>  0) & 0xff))

#define CR_A(c)       (((float)RGBA_A(c) / 255.0f))
#define CR_B(c)       (((float)RGBA_B(c) / 255.0f))
#define CR_G(c)       (((float)RGBA_G(c) / 255.0f))
#define CR_R(c)       (((float)RGBA_R(c) / 255.0f))

#define CR_ALPHA(c)   CR_A(c)
#define CR_BLUE(c)    CR_B(c)
#define CR_GREEN(c)   CR_G(c)
#define CR_RED(c)     CR_R(c)
