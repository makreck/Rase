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

char* App::load_config_json(const char* cmd) {
    const char* p = &cmd[strlen(TTY_KEY_API_CONFIG_PUT)];
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

    strcpy(command_string, TTY_KEY_API_CONFIG_PUT);
    ssize_t length = read(fdf, &command_string[strlen(command_string)], st.st_size);
    close(fdf);
    if (length != st.st_size) {
        free (command_string);
        printf("Error reading file, %d of %d bytes read!\n", (int)length, (int)st.st_size);
        return (nullptr);
    }

    return (command_string);
}

void App::handle_transaction_result(char* result) {
    if (result == nullptr) {
        return;
    }
    // printf("\033c");
    printf("\nResponse data:\n\"\"\"\n%s\"\"\"\n", result);
}

void App::get_options(void) {
    m.flags.w = 0;

    int n = 0;
    for (int i = 1; i < m.argc; i++) {
        const char* p = m.argv[i];
        if (p == nullptr) continue;
        if (p[0] == '-') {
            n++;
            if ((strcmp(p, "-a") == 0) || (strcmp(p, "--all") == 0)) {
                m.flags.b.all_devices = 1;
            } else if ((strcmp(p, "-t") == 0) || (strcmp(p, "--tty") == 0)) {
                m.flags.b.use_tty = 1;
            } else if ((strcmp(p, "-i") == 0) || (strcmp(p, "--ip") == 0)) {
                m.flags.b.use_ip = 1;
            } else if ((strcmp(p, "-h") == 0) || (strcmp(p, "--help") == 0)) {
                print_help();
            } else {
                printf("Error, unknown option \"%s\"\n", p);
            }
        }
    }

    m.flags.b.n_options = (uint32_t)n;

    if (m.flags.w == 0) {
        m.flags.b.use_tty = 1;
    }
}

void App::find_interfaces(void) {
    if (m.flags.b.use_ip == 1) {
        m.ip_device.start_scan(&m.device_list, &m.device_list_mutex);
    }

    if (m.flags.b.use_tty == 1) {
        EspTool::find_tty_devices(&m.device_list, &m.device_list_mutex);
    }

    m.ip_device.wait_for_scan();
}

void App::run_command(void) {
    get_options();
    find_interfaces();

    if (m.device_list.size() < 1) {
        printf("Error: No matching interface (\"ttyACM<n>\" or \"ttyUSB<n>\" or \"192.168.178.x\") found, plase connect a device!\n");
        return;
    }

    for (int i = 1; i < m.argc; i++) {
        const char* p = m.argv[i];
        if (p == nullptr) continue;
        if (p[0] == '/') {
            run_single_command(p);
        }
    }
}

void App::run_single_command(const char* _cmd) {

    for (DevConfig*& entry : m.device_list) {
        if (entry == nullptr) continue;

        char* cmd_string = nullptr;
        const char* p = strstr(_cmd, TTY_KEY_API_CONFIG_PUT);
        if (p != nullptr) {
            cmd_string = load_config_json(p);
        } else if (strstr(_cmd, "/") != nullptr) {
            cmd_string = EspTool::allocate_command(_cmd);
        }

        if (cmd_string != nullptr) {
            const char* ifac = nullptr;

            if (m.flags.b.all_devices == 1) {
                if ((entry->tty_ifac[0] != 0) && (strstr(_cmd, WEB_KEY_API_UPDATE) == nullptr)) {
                    ifac = entry->tty_ifac;
                } else if (entry->ip_ifac[0] != 0) {
                    ifac = entry->ip_ifac;
                }
            } else if (m.flags.b.use_tty == 1) {
                if ((entry->tty_ifac[0] != 0) && (strstr(_cmd, WEB_KEY_API_UPDATE) == nullptr)) {
                    ifac = entry->tty_ifac;
                }
            } else if (m.flags.b.use_ip == 1) {
                if (entry->ip_ifac[0] != 0) {
                    ifac = entry->ip_ifac;
                }
            }

            printf("--> Performing command \"%s\" on interface \"%s\"\n", _cmd, ifac); // ****

            char* response = EspTool::transact_command(ifac, cmd_string);
            free(cmd_string);

            if (response != nullptr) {
                handle_transaction_result(response);
                free(response);
            }
        }
    }
}
