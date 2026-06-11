/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
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

#define RGBA(_r, _g, _b, _a) (ColorRef)((((int)(_a) & 0xff) << 24) | (((int)(_b) & 0xff) << 16) | (((int)(_g) & 0xff) << 8) | (((int)(_r) & 0xff) << 0))
#define RGB_ALPHA(color, alpha) (((color) & 0x00ffffff) | (((alpha) & 0xff) << 24))

#define C_WHITE        (RGBA(255, 255, 255, 255))
#define C_BLACK        (RGBA(  0,   0,   0, 255))
#define C_TRANSPARENT  (RGBA(  0,   0,   0,   0))
#define C_RED          (RGBA(255,   0,   0, 255))
#define C_GREEN        (RGBA(  0, 255,   0, 255))
#define C_BLUE         (RGBA(  0,   0, 255, 255))
#define C_YELLOW       (RGBA(255, 255,   0, 255))
#define C_CYAN         (RGBA(  0, 255, 255, 255))
#define C_MAGENTA      (RGBA(255,   0, 255, 255))

#define RGBA_A(c)      ((uint8_t)(((uint32_t)(c) >> 24) & 0xff))
#define RGBA_B(c)      ((uint8_t)(((uint32_t)(c) >> 16) & 0xff))
#define RGBA_G(c)      ((uint8_t)(((uint32_t)(c) >>  8) & 0xff))
#define RGBA_R(c)      ((uint8_t)(((uint32_t)(c) >>  0) & 0xff))

#define CR_A(c)        (((float)RGBA_A(c) / 255.0f))
#define CR_B(c)        (((float)RGBA_B(c) / 255.0f))
#define CR_G(c)        (((float)RGBA_G(c) / 255.0f))
#define CR_R(c)        (((float)RGBA_R(c) / 255.0f))

#define CR_ALPHA(c)    CR_A(c)
#define CR_BLUE(c)     CR_B(c)
#define CR_GREEN(c)    CR_G(c)
#define CR_RED(c)      CR_R(c)

#define CBF_RED        (0.9f)
#define CBF_GREEN      (1.3f)
#define CBF_BLUE       (0.7f)
#define CBF_SUM        (CBF_RED + CBF_GREEN + CBF_BLUE)

#define CBF_THRFESHOLD (0.5f)

#define _cairo_set_source_rgb(hdc, colorref) cairo_set_source_rgb(hdc, CR_RED(colorref), CR_GREEN(colorref), CR_BLUE(colorref));
#define _cairo_set_source_rgba(hdc, colorref) cairo_set_source_rgba(hdc, CR_RED(colorref), CR_GREEN(colorref), CR_BLUE(colorref), CR_ALPHA(colorref));
#define _cairo_set_source_rgb_a(hdc, colorref, alpha) cairo_set_source_rgba(hdc, CR_RED(colorref), CR_GREEN(colorref), CR_BLUE(colorref), std::min(1.0f, std::max(0.0f, (float)(alpha))));

#define RGBA_WITH_ALPHA(c, a)  ((ColorRef)(((uint32_t)(c) & 0x00ffffff) |  ((uint32_t)(255.0 * (a)) << 24)))

class ColorRGBA {
    public:
        union {
            ColorRef ref;
            struct {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
            };
        };

        ColorRGBA(void) {
            ref = 0;
        }

        ColorRGBA(ColorRef _source) {
            ref = _source;
        }

        ColorRGBA(int _red, int _green, int _blue, int _alpha = 255) {
            r = _red;
            g = _green;
            b = _blue;
            a = _alpha;
        }

        void operator=(const ColorRGBA& _src) {
            ref = _src.ref;
        }

        void operator=(const ColorRef _src) {
            ref = _src;
        }

        void set(float _red, float _green, float _blue, float _alpha = 1.0f) {
            r = (uint8_t)(std::max(0.0f, std::min(1.0f, _red)) * 255.0f);
            g = (uint8_t)(std::max(0.0f, std::min(1.0f, _green)) * 255.0f);
            b = (uint8_t)(std::max(0.0f, std::min(1.0f, _blue)) * 255.0f);
            a = (uint8_t)(std::max(0.0f, std::min(1.0f, _alpha)) * 255.0f);
        }

        void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 0xff) {
            r = _r;
            g = _g;
            b = _b;
            a = _a;
        }

        ColorRef get(void) {
            return (ref);
        } 

        float red(void) {
            return ((float)(r) / 255.0f);
        }

        float green(void) {
            return ((float)(g) / 255.0f);
        }

        float blue(void) {
            return ((float)(b) / 255.0f);
        }

        float alpha(void) {
            return ((float)(a) / 255.0f);
        }

        ColorRef mul(float _factor) {
            float c_red   = std::max(0.0f, std::min(1.0f, (red()   * _factor))) * 255.0f;
            float c_green = std::max(0.0f, std::min(1.0f, (green() * _factor))) * 255.0f;
            float c_blue  = std::max(0.0f, std::min(1.0f, (blue()  * _factor))) * 255.0f;
            return (RGBA(c_red, c_green, c_blue, a));
        }

        float get_brightness(void) {
            return (std::max(0.0f, std::min(1.0f, ((red() * CBF_RED) + (green() * CBF_GREEN) + (blue() * CBF_BLUE)) / CBF_SUM)));
        }

        ColorRef get_text_overlay_color(double _threshold = CBF_THRFESHOLD) {
            return ((get_brightness() >= _threshold) ? C_BLACK : C_WHITE);
        }

};
