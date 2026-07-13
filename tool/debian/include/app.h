/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,      Preliminary Configuration Tool
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

class App {
    private:
        struct {
            int argc = 0;
            char** argv = nullptr;
            int fd = -1;
            char ifac[32]{ 0 };
            char* command_string = nullptr;
            char* response_data = nullptr;
        } m;

        static void print_help(void);

        void init(int argc, char* argv[]);
        void cleanup(void);

        bool find_interface(void);
        bool close_interface(void);

        int open_port(const char* ifac, speed_t baudrate);

        void run_gui(void);

        void run_command(void);
        bool delete_command_string(void);
        bool delete_response_data(void);
        bool load_config_json(char* cmd);
        bool alloc_command(char* cmd);
        bool transact_command(void);
        void handle_transaction_result(void);

    public:
        App(int argc, char* argv[]) {
            init(argc, argv);
        }

        ~App() {
            cleanup();
        }

        void run(void);
};