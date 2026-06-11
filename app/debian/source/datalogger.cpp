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

#include "includes.h"

void Datalogger::create_path(const char* _folder_path, ProductID* _product_id, std::string& _folder, std::string& _filename, std::string& _path) {
    if (_product_id != nullptr) {

        if (_folder_path == nullptr) {
            Files::get_home_dir(_folder, LOG_FILE_DEF_FOLDER);
        } else {
            _folder = _folder_path;
        }

        _filename = _product_id->device_serial_number;
        _filename.append(LOG_FILE_EXTENSION);

        _path = _folder;
        _path.append("/");
        _path.append(_filename);

        Files::mkpath(_folder.c_str(), 0777);
    }
}

void Datalogger::init(const char* _folder_path, ProductID* _product_id, std::vector<Scale*>* _channels) {
    m.fd = -1;
    m.logfile = new LogFile();

    create_path(_folder_path, _product_id);
    open_file(_product_id, _channels);

    uint64_t update_threshold_ms = m.logfile->get_header()->get_auto_update_threshold_ms();
    if (update_threshold_ms > 0) {
        m.auto_update_timestamp = Times::get_tick_count64() + update_threshold_ms;
    } else {
        m.auto_update_timestamp = 0;
    }
}

void Datalogger::cleanup(void) {
    if ((m.logfile != nullptr) && (m.fd != -1)) {
        m.logfile->put(m.fd);
    }

    if (m.logfile != nullptr) {
        // @todo Still problems with "Scale" class, probably with virtual functions for MicroJson handler.
        // delete (m.logfile); 
    }

    if (m.fd != -1) {
        Files::close_file(m.fd);
    }
}

void Datalogger::create_path(const char* _folder_path, ProductID* _product_id) {
    Datalogger::create_path(_folder_path, _product_id, m.folder, m.filename, m.path);
}

const char* Datalogger::get_path(void) {
    return (m.path.c_str());
}

void Datalogger::update_header(ProductID* _product_id) {
    if (_product_id != nullptr) {
        m.logfile->get_header()->set(_product_id);
    }
}

void Datalogger::update_inventory(std::vector<Scale*>* _channels) {
    if (_channels != nullptr) {
        LogInventory* inventory = m.logfile->get_inventory();
        inventory->clear();
        if (_channels != nullptr) {
            for (Scale*& slot : *_channels) {
                inventory->add_slot(slot);
            }
        }
    }
}

void Datalogger::open_file(ProductID* _product_id, std::vector<Scale*>* _channels) {
    if (Files::is_existing(m.path.c_str())) {
        if (Files::open_file(m.fd, m.path.c_str(), O_RDWR)) {
            m.logfile->get(m.fd);
            update_inventory(_channels);
        }
    } else {
        if (Files::open_file(m.fd, m.path.c_str(), O_RDWR)) {
            update_header(_product_id);
            update_inventory(_channels);
            m.logfile->get_registry()->clear();
            m.logfile->put(m.fd);
        }
    }

    m.auto_update_timestamp = Times::get_tick_count64() 
                            + m.logfile->get_header()->get_auto_update_threshold_ms();
}

bool Datalogger::add_measurement(std::vector<Scale*>* _channels) {
    if (_channels == nullptr) {
        return (false);
    }

    double timecode = Times::get_now();
    if (_channels != nullptr) {
        LogInventory* inventory = m.logfile->get_inventory();
        for (Scale*& slot : *_channels) {
            int slot_index = inventory->add_slot(slot);
            if (slot_index != -1) {
                LogFrame frame(slot_index, timecode, slot->get_value());
                if (m.logfile->put_frame(m.fd, &frame)) {

// printf("Put frame %lld: slot=%d, timecode=%.8f, value=%.1f\n",
//     (long long int)m.logfile->get_registry()->get_count_of_records(), frame.get_slot(), frame.get_timecode(), frame.get_value());

                    check_for_update();
                }
            }
        }
    }

    return (true);
}

void Datalogger::check_for_update(void) {
    if (m.auto_update_timestamp == 0) {
        return;
    }

    LogHeader* header = m.logfile->get_header();
    uint64_t update_threshold_ms = header->get_auto_update_threshold_ms();

    if (update_threshold_ms == 0) {
        return;
    }

    uint64_t timestamp = Times::get_tick_count64();
    if (timestamp >= m.auto_update_timestamp) {
        m.auto_update_timestamp += update_threshold_ms;

        if (header->is_modified()) {
            m.logfile->put(m.fd);
        }
    }
}
