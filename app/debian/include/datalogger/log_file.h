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

class LogFile {
    private:
        LogHeader*    header;       //   1024 bytes   1K
        LogInventory* inventory;    //  10880 bytes  15K
        LogRegistry*  registry;     // 131136 bytes 112K -> 114688 K = 7168 chunks

    public:
        LogFile(void) {
            header    = new LogHeader();
            inventory = new LogInventory();
            registry  = new LogRegistry();
        }

        ~LogFile() {
            if (header != nullptr) {
                delete (header);
                header = nullptr;
            }

            if (inventory != nullptr) {
                // Since inventory consists of Scale objects which have virtual functions,
                // this is causing a crash because the content was loaded from a file and
                // the virtual functions were not restored correctly here.
                // 
                // @TODO: Split Scale object into a JsonScale and a Scale object. Scale
                //        object must not have virtual functions. JSON import must use
                //        a JasonScale object which after parsing the JSON data can be
                //        imported into the Scale object. Then, the Scale objects may
                //        be loaded from the file.
                //
                // Currently, this is causing a memory leaking !
                // delete (inventory);
                inventory = nullptr;
            }

            if (registry != nullptr) {
                delete (registry);
                registry = nullptr;
            }
        }

        LogHeader*    get_header(void);
        LogInventory* get_inventory(void);
        LogRegistry*  get_registry(void);

        bool put(int _fd);
        bool get(int _fd);
        
        int64_t put_frame(int _fd, LogFrame* _frame);
        bool get_frame(int _fd, int64_t _file_position, LogFrame* _frame);
};
