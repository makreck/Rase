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

typedef uint32_t color_t;

enum class ColorRef : color_t {
    RED     = 0xff0000ff,
    GREEN   = 0xff00ff00,
    BLUE    = 0xffff0000,
    
    YELLOW  = (RED | GREEN),
    MAGENTA = (RED | BLUE),
    CYAN    = (BLUE | GREEN),
    
    ORANGE  = 0xff007fff,

    WHITE   = (RED | GREEN | BLUE),
    BLACK   = 0x00000000,
};

class Color {
    public:
        union __attribute__ ((packed)) {
            uint32_t c;
            uint8_t rgba[4];
            struct {
                uint8_t R;
                uint8_t G;
                uint8_t B;
                uint8_t A;
            };
        };

        Color(const uint32_t value = 0) {
            c = value;
        }

        Color(const Color& source) {
            c = source.c;
        }

        Color(const ColorRef ref) {
            c = (uint32_t)ref;
        }

        static Color getGreenToRedLevel(float normFactor);

        void clear(void);
        void set_rgb(float red, float green, float blue);
        void set_rgba(float red, float green, float blue, float alpha);
        void set_green_to_red_level(float normFactor);
        void set_color(ColorRef color);
        void set_gray(float gray, float alpha = 1.0f);

        float get_red(void);
        float get_green(void);
        float get_blue(void);
        float get_alpha(void);
        float get_gray(void);
        float get_max(void);


};
