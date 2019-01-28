/*
 * \file serial.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "serial.hh"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sstream>

#include <time.h>

SerialInterface::SerialInterface(const SerialConfig_t sConfig)
{
    m_serial = open(sConfig.port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (m_serial < 0) {
        return;
    }

    if (setAttributes(sConfig.baudrate)) printf("Error Setting Attributes!\n");
}

SerialInterface::~SerialInterface()
{
    close(m_serial);
}

int SerialInterface::setAttributes(speed_t baudrate)
{
    // Initialize configuration struct with current configuration
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(m_serial, &tty) != 0) {
        return -1;
    }

    // Set the baudspeed of the serial port
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag &= (~ICANON & ~ECHO);
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);

    // Set Stopbyte / byte size of serial port
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    // Apply configuration
    if (tcsetattr(m_serial, TCSANOW, &tty) != 0) {
        return -1;
    }

    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(m_serial, &tty) != 0) {
        return -1;
    }

    // Make serial port non-blocking
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(m_serial, TCSANOW, &tty) != 0) {
        return -1;
    }

    return 0;
}

std::string SerialInterface::getNextLine()
{
    std::string str;

    char buffer = 0;
    int n;
    while (true) {
        n = read(m_serial, &buffer, 1);
        if (n) {
            if (buffer == '\n') {
                break;
            } else {
                str += buffer;
            }
        }
    }

    return str;
}
