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

class SDCard {
    private:
        struct {
            struct {
                sdmmc_slot_config_t slot_config;
                sdmmc_host_t host;
                esp_vfs_fat_sdmmc_mount_config_t mount_config;
            } setup;

            sdmmc_card_t *card = nullptr;
            std::vector<char*> file_list;
        } m;

        void init(void);
        void cleanup(void);
        void clear_file_list(void);

    public:
        SDCard() {
            init();
        }

        ~SDCard() {
            cleanup();
        }

        std::vector<char*> list_files(void);
        ssize_t get_file_size(const char* _filename);

};
