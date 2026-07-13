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

#include "includes.h"

void App::init(int argc, char* argv[]) {
    m.argc = argc;
    m.argv = argv;
}

void App::cleanup(void) {
    close_interface();
    delete_command_string();
    delete_response_data();
}

void App::print_help(void) {
    printf("Rase Sensor Commander, V0.0.1, (C)2026\n");
    printf("*** Important: Please ensure, that sensor's serial interface is enabled by sensor setup! ***\n");
    printf("Usage: fcmd [options] command\n");
    printf("Supported commands:\n");
    printf("\t\"/connect=<ssid>:<password>\"\tConnect to a WLAN access point.\n");
    printf("\t\"/broker=<MQTT broker>:<username>:<password>\"\tConfigure a MQTT broker/server link.\n");
    printf("\t\"/config=<JSON.file>\"\t\tConfigure device by given JSON file.\n");
    printf("\t\"/initialize\"\t\t\tPerform a factory reset.\n");
    printf("\t\"/reboot\"\t\t\tReboot the device.\n");
    printf("\t\"/root\"\t\t\t\tQuery the root website source.\n");
    printf("\t\"/api/id\"\t\t\tQuery the senosr identification data (JSON).\n");
    printf("\t\"/api/sensors\"\t\t\tQuery the latest measurement (JSON).\n");
}

void App::run(void) {
    if (m.argc < 2) {
        run_gui();
    } else {
        run_command();
    }
}

int main(int argc, char* argv[]) {
    App* app = new App(argc, argv);
    if (app != nullptr) {
        app->run();
        delete (app);
    }

    return (0);
}
