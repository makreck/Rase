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

char* App::load_config_json(char* cmd) {
    char* p = &cmd[8];
    char filename[PATH_MAX]{0};
    for (int i = 0; (i < (PATH_MAX - 1) && (*p > ' ')); i++) {
        filename[i] = *p++;
    }

    struct stat st{0};
    if (lstat(filename, &st) == -1) {
        printf("Error, file <%s> not found!\n", filename);
        return (nullptr);
    }

    ssize_t size = st.st_size + 32;
    char* command_string = (char *)malloc(size);
    if (command_string == nullptr) {
        printf("Error, unable to alloc %d bytes for file <%s>!\n", (int)size, filename);
        return (nullptr);
    }
    memset(command_string, 0, size);

    int fdf = open(filename, O_RDONLY);
    if (fdf == -1) {
        free (command_string);
        printf("Error opening file <%s> for input!\n", filename);
        return (nullptr);
    }

    strcpy(command_string, "/config=");
    ssize_t length = read(fdf, &command_string[strlen(command_string)], st.st_size);
    close(fdf);
    if (length != st.st_size) {
        free (command_string);
        printf("Error reading file, %d of %d bytes read!\n", (int)length, (int)st.st_size);
        return (nullptr);
    }

    return (command_string);
}

void App::run_command(char* cmd) {
    if (!DevConfig::find_interface(m.ifac, sizeof (m.ifac))) {
        printf("No interface (\"ttyACM<n>\" ot \"ttyUSB<n>\") found, plase connect a device!\n");
        return;
    }

    if ((strstr(cmd, "--help") != nullptr) || (strstr(cmd, "-H") != nullptr)) {
        print_help();
        return;
    } else if (strlen(cmd) < 1) {
        run_gui();
    }

    char* cmd_string = nullptr;
    char* p = strstr(cmd, "/config=");
    if (p != nullptr) {
        cmd_string = load_config_json(p);
    } else if (strstr(cmd, "/") != nullptr) {
        cmd_string = DevConfig::allocate_command(cmd);
    }

    if (cmd_string != nullptr) {
        char* response = DevConfig::transact_command(m.ifac, cmd_string);
        free(cmd_string);

        if (response != nullptr) {
            handle_transaction_result(response);
            free(response);
        }
    }
}

void App::handle_transaction_result(char* result) {
    if (result == nullptr) {
        return;
    }
    // printf("\033c");
    printf("\nResponse data:\n\"\"\"\n%s\"\"\"\n", result);
}
