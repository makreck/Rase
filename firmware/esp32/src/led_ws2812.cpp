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

#include "app.hpp"

//#define DISPLAY_STATE

void WS2812::init(gpio_num_t _gpio_num, int _pixel_count, rmt_channel_t _channel, ColorOrder _color_order) {
	pixelCount  = _pixel_count;
	channel     = _channel;
	color_order = _color_order;
	itemCount   = pixelCount * 24 + 1;
	items       = (NeoPixelItem*)malloc(sizeof (NeoPixelItem) * itemCount);
	pixels      = (Color*)malloc(sizeof (Color) * pixelCount);

	clear();

	rmt_config_t config;
	memset(&config, 0, sizeof (config));
	config.rmt_mode = RMT_MODE_TX;
	config.channel = _channel;
	config.gpio_num = _gpio_num;
	config.mem_block_num = RMT_CHANNEL_MAX - channel;
	config.clk_div = NEOPIXEL_WS2812_CLK_DIV;
	config.tx_config.loop_en = false;
	config.tx_config.carrier_en = false;
	config.tx_config.idle_output_en = true;
	config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
	rmt_config(&config);
	rmt_driver_install(config.channel, 0, 0);
}

void WS2812::cleanup(void) {
	free(items);
	free(pixels);
}

void WS2812::translate(Color& nativeColor, Color color, float intensity, ColorOrder color_order) {
	if ((intensity <= 0.0f) || (color.A == 0) || (color.c == 0)) {
		nativeColor.clear();
	} else {
		float f = MAX(0.01f, MIN(1.0f, intensity));
		if (color_order == ColorOrder::BGR) {			// BGR: ESP32_S3_ZERO
			nativeColor.R = (uint8_t)(color.B * f);
			nativeColor.G = (uint8_t)(color.G * f);
			nativeColor.B = (uint8_t)(color.R * f);
		} else if (color_order == ColorOrder::BRG) {	// BRG: ESP32_S3_WROOM_1, ESP32_WROOM_DEV, ESP32_WROVER_DEV, ESP32_S3_SUPER_MINI
			nativeColor.R = (uint8_t)(color.B * f);
			nativeColor.G = (uint8_t)(color.R * f);
			nativeColor.B = (uint8_t)(color.G * f);
		} else {										// RGB: All others
			nativeColor.R = (uint8_t)(color.R * f);
			nativeColor.G = (uint8_t)(color.G * f);
			nativeColor.B = (uint8_t)(color.B * f);
		}
	}
}

void WS2812::set_pixel(int index, Color pixel) {
	if ((index >= 0) && (index < pixelCount)) {
		pixels[index].c = pixel.c;
	}
}

void WS2812::set_all(Color pixel) {
	for (int i = 0; i < pixelCount; i++) {
		modified |= (pixels[i].c != pixel.c);
		pixels[i].c = pixel.c;
	}
}

void WS2812::clear(void) {
	for (uint16_t i = 0; i < pixelCount; i++) {
		modified |= (pixels[i].c != 0);
		pixels[i].c = 0;
	}
}

void WS2812::set_color_order(ColorOrder _color_order) {
	color_order = _color_order;
}

size_t WS2812::get_count(void) {
	return (pixelCount);
}

void WS2812::set(Color color, float intensity, int index) {
	Color nativeColor;
	WS2812::translate(nativeColor, color, intensity, color_order);
	if (index < 0) {
		set_all(nativeColor);
	} else {
		set_pixel(index, nativeColor);
	}
}

void WS2812::apply(void) {
	if (modified) {
		modified = false;
		NeoPixelItem* pCurrentItem = items;
		for (int i = 0; i < pixelCount; i++) {
			uint32_t currentPixel = pixels[i].c;
#ifdef DISPLAY_STATE
			ESP_LOGI(TAG, "WS2812::apply() -> pixel = #%d, color = 0x%-8.8X ", i, (unsigned int)currentPixel);
#endif
			for (int8_t j = 23; j >= 0; j--) {
				if (currentPixel & (1 << j)) {
					pCurrentItem->setHigh();
				} else {
					pCurrentItem->setLow();
				}
				pCurrentItem++;
			}
		}
		pCurrentItem->setTerm();
		rmt_write_items(channel, items, itemCount, true);
	}
}


