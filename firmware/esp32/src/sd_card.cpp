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

#define DISPLAY_STATE

#define SD_CARD_MAX_FILES   (5)
#define SD_CARD_UNIT_SIZE   (16 * 1024)
#define SD_CARD_IFAC_WIDTH  (1)
#define SD_CARD_TIMEOUT     (3000)
#define SD_CARD_MOUNT_POINT "/sdcard"

void SDCard::init(void) {
#ifdef _ENABLE_SD_CARD

    memset(&m.setup, 0, sizeof (m.setup));

    m.setup.slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    m.setup.slot_config.cmd   = SD_CARD_CMD;
    m.setup.slot_config.clk   = SD_CARD_CLK;
    m.setup.slot_config.d0    = SD_CARD_DATA;
    m.setup.slot_config.d1    = GPIO_NUM_NC;
    m.setup.slot_config.d2    = GPIO_NUM_NC;
    m.setup.slot_config.d3    = GPIO_NUM_NC;
    m.setup.slot_config.d4    = GPIO_NUM_NC;
    m.setup.slot_config.d5    = GPIO_NUM_NC;
    m.setup.slot_config.d6    = GPIO_NUM_NC;
    m.setup.slot_config.d7    = GPIO_NUM_NC;
    m.setup.slot_config.cd    = SDMMC_SLOT_NO_CD;
    m.setup.slot_config.wp    = SDMMC_SLOT_NO_WP;
    m.setup.slot_config.width = SD_CARD_IFAC_WIDTH;
    m.setup.slot_config.flags = 0;

    m.setup.host = SDMMC_HOST_DEFAULT();
    m.setup.host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    
    m.setup.mount_config.format_if_mount_failed = true;
    m.setup.mount_config.max_files              = SD_CARD_MAX_FILES;
    m.setup.mount_config.allocation_unit_size   = SD_CARD_UNIT_SIZE;
    
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(SD_CARD_MOUNT_POINT, &m.setup.host, &m.setup.slot_config, &m.setup.mount_config, &m.card);
    if (ret != ESP_OK) {
#ifdef DISPLAY_STATE        
        ESP_LOGE(TAG, "SD-Card error \"%s\" ", esp_err_to_name(ret));
#endif        
        return;
    }
    
#ifdef DISPLAY_STATE        
    ESP_LOGI(TAG, "SD-Card info:");
    sdmmc_card_print_info(stdout, m.card);

    std::vector<char*> list = list_files();    // **** for testing only!
    ESP_LOGI(TAG, "SD-Card files:");
    for (char*& entry : list) {
        ssize_t file_size = get_file_size(entry);
        ESP_LOGI(TAG, " %s \t%d bytes", entry, (int)file_size);
    }
#endif

#endif    
}

void SDCard::cleanup(void) {
    clear_file_list();

    DIR* dir = opendir(SD_CARD_MOUNT_POINT);
    if (dir != nullptr) {
        closedir(dir);
        esp_vfs_fat_sdcard_unmount(SD_CARD_MOUNT_POINT, m.card);
        sdmmc_host_deinit();
    }
}

void SDCard::clear_file_list(void) {
    if (m.file_list.size() > 0) {
        for (char*& entry : m.file_list) {
            if (entry != nullptr) {
                free (entry);
                entry = nullptr;
            }
        }
    }
    m.file_list.clear();
}

std::vector<char*> SDCard::list_files(void) {
    clear_file_list();

    DIR* dir = opendir(SD_CARD_MOUNT_POINT);
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            size_t len = strlen(SD_CARD_MOUNT_POINT) + strlen(entry->d_name) + 2;
            char* s = (char*)malloc(len);
            if (s != nullptr) {
                snprintf(s, len, "%s/%s", SD_CARD_MOUNT_POINT, entry->d_name);
                s[len - 1] = '\0';
                m.file_list.push_back(s);
            }
        }
        closedir(dir);
    }    

    return (m.file_list);
}

ssize_t SDCard::get_file_size(const char* _filename) {
    struct stat file_stat;
    memset(&file_stat, 0, sizeof (file_stat));
    if (stat(_filename, &file_stat) == 0) {
        return ((ssize_t)file_stat.st_size);
    }
    return (-1);
}
