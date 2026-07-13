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

bool App::delete_command_string(void) {
    if (m.command_string != nullptr) {
        free(m.command_string);
        m.command_string = nullptr;
        return (true);
    }
    return (false);
}

bool App::delete_response_data(void) {
    if (m.response_data != nullptr) {
        free(m.response_data);
        m.response_data = nullptr;
        return (true);
    }
    return (false);
}

bool App::load_config_json(char* cmd) {
    char* p = &cmd[8];
    char filename[PATH_MAX]{0};
    for (int i = 0; (i < (PATH_MAX - 1) && (*p > ' ')); i++) {
        filename[i] = *p++;
    }

    struct stat st{0};
    if (lstat(filename, &st) == -1) {
        printf("Error, file <%s> not found!\n", filename);
        return (false);
    }

    ssize_t size = st.st_size + 32;
    char* command_string = (char *)malloc(size);
    if (command_string == nullptr) {
        printf("Error, unable to alloc %d bytes for file <%s>!\n", (int)size, filename);
        return (false);
    }
    memset(command_string, 0, size);

    int fdf = open(filename, O_RDONLY);
    if (fdf == -1) {
        free (command_string);
        printf("Error opening file <%s> for input!\n", filename);
        return (false);
    }

    strcpy(command_string, "/config=");
    ssize_t length = read(fdf, &command_string[strlen(command_string)], st.st_size);
    close(fdf);
    if (length != st.st_size) {
        free (command_string);
        printf("Error reading file, %d of %d bytes read!\n", (int)length, (int)st.st_size);
        return (false);
    }

    m.command_string = command_string;
    return (true);
}

bool App::alloc_command(char* cmd) {
    ssize_t size = strlen(cmd) + 2;
    char* command_string = (char *)malloc(size);
    if (command_string == nullptr) {
        printf("Error, unable to alloc %d bytes for command string!\n", (int)size);
        return (false);
    }
    memset(command_string, 0, size);
    strncpy(command_string, cmd, size - 1);

    m.command_string = command_string;
    return (true);
}

void App::run_command(void) {
    delete_command_string();
    char* cmd = m.argv[1];
    if (cmd == nullptr) {
        return;
    }

    char* p = strstr(cmd, "/config=");
    if (p != nullptr) {
        if (load_config_json(p)) {
            if(transact_command()) {
                handle_transaction_result();
            }
        }
    } else if (strstr(cmd, "/") != nullptr) {
        if (alloc_command(cmd)) {
            if(transact_command()) {
                handle_transaction_result();
            }
        }
    } else if ((strstr(cmd, "--help") != nullptr) || (strstr(cmd, "-H") != nullptr)) {
        print_help();
    } else {
        run_gui();
    }
}

bool App::transact_command(void) {
    if (m.command_string == nullptr) {
        printf("Error, tranaction not prepared!");
        return (false);
    }

    delete_response_data();

    ssize_t size_out = strlen(m.command_string);
    ssize_t len_out = write(m.fd, m.command_string, size_out);
    if (size_out != len_out) {
        printf("Error, unable to transmit %d bytes command string, %d bytes written!\n", (int)size_out, (int)len_out);
        return (false);
    }

    usleep(500000);
    
    ssize_t size = 65536;
    char* in = (char*)malloc(size);
    if (in == nullptr) {
        printf("Error, unable to allocate response data buffer!\n");
        return (false);
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
        return (false);
    }

    char* result = in;
    char* p = strstr(in, "<!DOCTYPE html>");
    if (p != nullptr) {
        result = p;
        char* p = strstr(in, "</html>");
        if (p != nullptr) {
            p[7] = '\0';
        }
        length = strlen(result);
    } else {
        int level = -1;
        int i;
        for (i = 0; (i < length) && (level != 0); i++) {
            if (in[i] == '{') {
                if (level == -1) {
                    level = 1;
                    result = &in[i];
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

    m.response_data = (char *)malloc(length + 1);
    if (m.response_data == nullptr) {
        free(in);
        printf("Unable to allocate %d bytes for response data!\n", (int)(length + 1));
        return (false);
    }
    memset(m.response_data, 0, length + 1);
    strncpy(m.response_data, result, length);
    free(in);

    return (true);
}

void App::handle_transaction_result(void) {
    if (m.response_data == nullptr) {
        return;
    }
    printf("\033c");
    printf("Response data:\n\"\"\"\n%s\"\"\"\n", m.response_data);
}
