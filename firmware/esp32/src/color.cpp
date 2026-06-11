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

#include "includes.hpp"
#include "color.hpp"

void Color::clear(void) {
    c = 0;
}

void Color::set_rgb(float red, float green, float blue) {
    set_rgba(red, green, blue, 1.0f);
}

void Color::set_rgba(float red, float green, float blue, float alpha) {
    R = (uint8_t)MAX(0.0f, MIN(255.0f, red   * 255.0f));
    G = (uint8_t)MAX(0.0f, MIN(255.0f, green * 255.0f));
    B = (uint8_t)MAX(0.0f, MIN(255.0f, blue  * 255.0f));
    A = (uint8_t)MAX(0.0f, MIN(255.0f, alpha * 255.0f));
}

void Color::set_gray(float gray, float alpha) {
    set_rgba(gray, gray, gray, alpha);
}

void Color::set_green_to_red_level(float normFactor) {
    float f = MAX(0.0f, MIN(1.0f, fabsf(normFactor)));
    float green = sinf(f * (float)M_PI_2);
    float red = sinf((1.0f - f) * (float)M_PI_2);
    float cf = 1.0f / MAX(red, green);
    set_rgb(cf * red, cf * green, 0.0f);
}

Color Color::getGreenToRedLevel(float normFactor) {
    Color color;
    color.set_green_to_red_level(normFactor);
    return (color);
}

float Color::get_red(void) {
    return ((float)(R) / 255.0f);
}

float Color::get_green(void) {
    return ((float)(G) / 255.0f);
}

float Color::get_blue(void) {
    return ((float)(B) / 255.0f);
}

float Color::get_alpha(void) {
    return ((float)(A) / 255.0f);
}

float Color::get_gray(void) {
    return (((float)(R) + (float)(G) + (float)(B)) / 765.0f);
}

float Color::get_max(void) {
    return ((float)MAX(MAX(R, G), B) / 255.0f);
}        

