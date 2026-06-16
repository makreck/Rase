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

#include "datalogger/log_defs.h"
#include "datalogger/log_frame.h"
#include "datalogger/log_inventory.h"
#include "datalogger/log_registry.h"
#include "datalogger/log_header.h"
#include "datalogger/log_range.h"
#include "datalogger/log_window.h"
#include "datalogger/log_file.h"

class Datalogger {
    private:
        struct {
            std::string folder;
            std::string filename;
            std::string path;

            int fd = -1;
            LogFile* logfile = nullptr;

            uint64_t auto_update_timestamp = 0;
        } m;

        void init(const char* _folder_path, ProductID* _product_id, std::vector<Scale*>* _channels);
        void cleanup(void);

        void update_inventory(std::vector<Scale*>* _channels);
        void update_header(ProductID* _product_id);
        void create_path(const char* _folder_path, ProductID* _product_id);
        void open_file(ProductID* _product_id, std::vector<Scale*>* _channels);
        void check_for_update(void);

    public:
        Datalogger(const char* _folder_path, ProductID* _product_id = nullptr, std::vector<Scale*>* _channels = nullptr) {
            init(_folder_path, _product_id, _channels);
        }

        ~Datalogger() {
            cleanup();
        }

        static void create_path(const char* _folder_path, ProductID* _product_id, std::string& _folder, std::string& _filename, std::string& _path);

        bool add_measurement(std::vector<Scale*>* _channels);
        const char* get_path(void);

};
