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

    // Read possible dirt from the ESP32 Debug Output to avoid interference with commanding data!
    uint8_t dummy_in[4096];
    read(m.fd, dummy_in, sizeof (dummy_in));

    return (fd);
}

bool App::close_interface(void) {
    if (m.fd == -1) {
        return (false);
    }
    close(m.fd);
    m.fd = -1;
    memset(m.ifac, 0, sizeof (m.ifac));
    return (true);
}

bool App::find_interface(void) {
    int index = 0;
    do {
        snprintf(m.ifac, sizeof (m.ifac) - 1, "/dev/ttyACM%d", index);
        m.fd = open_port(m.ifac, 115200);
        if (m.fd >= 0) break;
        snprintf(m.ifac, sizeof (m.ifac) - 1, "/dev/ttyUSB%d", index);
        m.fd = open_port(m.ifac, 115200);
        if (m.fd >= 0) break;
    } while (++index < 10);
    return (m.fd >= 0);
}
