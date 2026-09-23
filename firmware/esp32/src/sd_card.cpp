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

void SDCard::init(void) {
#ifdef _ENABLE_SD_CARD

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.cmd   = SD_CARD_CMD;
    slot_config.clk   = SD_CARD_CLK;
    slot_config.d0    = SD_CARD_DATA;
    slot_config.d1    = GPIO_NUM_NC;
    slot_config.d2    = GPIO_NUM_NC;
    slot_config.d3    = GPIO_NUM_NC;
    slot_config.d4    = GPIO_NUM_NC;
    slot_config.d5    = GPIO_NUM_NC;
    slot_config.d6    = GPIO_NUM_NC;
    slot_config.d7    = GPIO_NUM_NC;
    slot_config.cd    = SDMMC_SLOT_NO_CD;
    slot_config.wp    = SDMMC_SLOT_NO_WP;
    slot_config.width = 1;
    slot_config.flags = 0;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    
    memset(&m.mount_config, 0, sizeof (m.mount_config));
    m.mount_config.format_if_mount_failed = true;
    m.mount_config.max_files = 5;
    m.mount_config.allocation_unit_size = 16 * 1024;
    
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &m.mount_config, &m.card);
    
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. If you have already mounted it, this is not an error");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s)", esp_err_to_name(ret));
        }
        return;
    }
    
    sdmmc_card_print_info(stdout, m.card);
#endif    
}

void SDCard::cleanup(void) {

}
