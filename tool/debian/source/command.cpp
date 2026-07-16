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

char* App::alloc_command(char* cmd) {
    ssize_t size = strlen(cmd) + 2;
    char* command_string = (char *)malloc(size);
    if (command_string == nullptr) {
        printf("Error, unable to alloc %d bytes for command string!\n", (int)size);
        return (nullptr);
    }
    memset(command_string, 0, size);
    strncpy(command_string, cmd, size - 1);

    return (command_string);
}

void App::run_command(void) {
    char* cmd = m.argv[1];
    if (cmd == nullptr) {
        return;
    }

    if (!find_interface()) {
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
        cmd_string = alloc_command(cmd);
    }

    if (cmd_string != nullptr) {
        char* response = transact_command(cmd_string);
        free(cmd_string);

        if (response != nullptr) {
            handle_transaction_result(response);
            free(response);
        }
    }
}

char* App::transact_command(const char* cmd) {
    if (cmd == nullptr) {
        printf("Error, tranaction not prepared!");
        return (nullptr);
    }

    if (!open_interface()) {
        return (nullptr);
    }

    ssize_t size_out = strlen(cmd);
    ssize_t len_out = write(m.fd, cmd, size_out);
    if (size_out != len_out) {
        printf("Error, unable to transmit %d bytes command string, %d bytes written!\n", (int)size_out, (int)len_out);
        return (nullptr);
    }

    usleep(500000);
    
    ssize_t size = 65536;
    char* in = (char*)malloc(size);
    if (in == nullptr) {
        printf("Error, unable to allocate response data buffer!\n");
        return (nullptr);
    }
    memset(in, 0, size);

    ssize_t len_read = 0;
    ssize_t length = 0;
    do {
        len_read = read(m.fd, &in[length], size - length - 1);
        if (len_read > 0) {
            length += len_read;
            usleep(25000);
        }
    } while (len_read > 0);
    in[length] = '\0';

    if (length == 0) {
        free(in);
        return (nullptr);
    }

    char* response = in;
    char* p = strstr(in, "<!DOCTYPE html>");
    if (p != nullptr) {
        response = p;
        char* p = strstr(in, "</html>");
        if (p != nullptr) {
            p[7] = '\0';
        }
        length = strlen(response);
    } else {
        int level = -1;
        int i;
        for (i = 0; (i < length) && (level != 0); i++) {
            if (in[i] == '{') {
                if (level == -1) {
                    level = 1;
                    response = &in[i];
                } else {
                    level++;
                }
            }
            if (in[i] == '}') {
                level--;                
            }
        }
        in[i++] = '\r';
        in[i++] = '\n';
        in[i] = '\0';
        length = i;
    }

    char* result = (char *)malloc(length + 1);
    if (result == nullptr) {
        free(in);
        printf("Unable to allocate %d bytes for result data!\n", (int)(length + 1));
        return (nullptr);
    }
    memset(result, 0, length + 1);
    strncpy(result, response, length);
    free(in);

    close_interface();

    return (result);
}

void App::handle_transaction_result(char* result) {
    if (result == nullptr) {
        return;
    }
    printf("\033c");
    printf("Response data:\n\"\"\"\n%s\"\"\"\n", result);
}
