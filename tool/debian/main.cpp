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

#include <stdio.h>
#include <cstddef>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
#include "main.h"

int App::open_port(const char* ifac, speed_t baudrate) {
    if (ifac == nullptr) return (-1);

    int fd = open(ifac, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        return (fd);
    }

    termios terminal{ 0 };
    terminal.c_cflag = CS8 | CREAD;
    terminal.c_iflag = IGNCR | IGNBRK | IGNPAR | IXANY;
    terminal.c_oflag = 0;
    terminal.c_lflag = IEXTEN | CLOCAL | NOFLSH;
    terminal.c_cc[VTIME] = 1;
    terminal.c_cc[VMIN] = 1;

    cfsetspeed(&terminal, baudrate);
    tcsetattr(fd, TCSANOW, &terminal);
    tcflush(fd, TCIOFLUSH);

    return (fd);
}

void App::init(int argc, char* argv[]) {
    argumentsCount = argc;
    argumentsList = argv;
}

void App::tearDown(void) {
}

void App::run(void) {
    int index = 0;
    int fd = -1;
    char ifac[32]{ 0 };

    do {
        snprintf(ifac, sizeof (ifac) - 1, "/dev/ttyACM%d", index);
        fd = open_port(ifac, 115200);
        if (fd >= 0) break;
        
        snprintf(ifac, sizeof (ifac) - 1, "/dev/ttyUSB%d", index);
        fd = open_port(ifac, 115200);
        if (fd >= 0) break;

    } while (++index < 10);

    if (fd < 0) {
        printf("No interfaces found!\n");
        return;
    }

    bool mem_flag = false;
    char* cmd = argumentsList[1];
    char* p = strstr(cmd, "/config=");
    if (p != nullptr) {
        p = &p[8];
        char filename[PATH_MAX]{0};
        for (int i = 0; (i < (PATH_MAX - 1) && (*p > ' ')); i++) {
            filename[i] = *p++;
        }
        struct stat st{ 0 };
        if (lstat(filename, &st) != -1) {
            cmd = (char*)malloc(st.st_size + 32);
            if (cmd != nullptr) {
                int fdf = open(filename, O_RDONLY);
                if (fdf != -1) {
                    strcpy(cmd, "/config=");
                    read(fdf, &cmd[strlen(cmd)], st.st_size);
                    close(fdf);
                    mem_flag = true;
                    printf("Sending:\n\"\"\"\n%s\"\"\"\n", cmd);
                } else {
                    printf("Error opening file <%s>!\n", filename);
                }
            } else {
                printf("Error, unable to alloc %d bytes for file <%s>!\n", (int)(st.st_size + 32), filename);
            }
        } else {
            printf("Error, file <%s> not found!\n", filename);
        }
    }

    uint8_t dummy_in[4096];
    read(fd, dummy_in, sizeof (dummy_in));

    ssize_t len_out = write(fd, cmd, strlen(cmd));

    usleep(500000);
    
    size_t size = 32768;
    char* in = (char*)malloc(size + 8);

    ssize_t len_in = 0;
    ssize_t length = 0;
    printf("\n");
    do {
        memset(in, 0, size);
        len_in = read(fd, in, size);
        if (len_in > 0) {
            in[len_in] = 0;
            length += len_in;
            printf("%s", (char*)in);
            usleep(25000);
        }
    } while (len_in > 0);
    printf("\n");

    free(in);

    if (length < 1) {
        printf("No response from device received. Please check, if serial interface is enabled!\n");
    } else {
        printf("\n%d bytes received.\n", (int)length);
    }

    close(fd);

    if (mem_flag) {
        free(cmd);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Rase Sensor Commander, V0.0.1, (C)2026\n");
        printf("*** Important: Please ensure, that sensor's serial interface is enabled by sensor setup! ***\n");
        printf("Usage: fcmd [options] command\n");
        printf("Supported commands:\n");

        printf("\t\"/connect=<ssid>:<password>\"\tConnect to a WLAN access point.\n");
        printf("\t\"/broker=<MQTT broker>:<username>:<password>\"\tConfigure a MQTT broker/server link.\n");
        printf("\t\"/config=<JSON.file>\"\t\tConfigure device by given JSON string.\n");
        printf("\t\"/initialize\"\t\t\tPerform a factory reset.\n");
        printf("\t\"/reboot\"\t\t\tReboot the device.\n");
        printf("\t\"/root\"\t\t\t\tQuery the root website source.\n");
        printf("\t\"/api/id\"\t\t\tQuery the senosr identification data (JSON).\n");
        printf("\t\"/api/sensors\"\t\t\tQuery the latest measurement (JSON).\n");

        return (0);
    }

    App* app = new App(argc, argv);
    if (app != nullptr) {
        app->run();
        delete (app);
    }

    return (0);
}
