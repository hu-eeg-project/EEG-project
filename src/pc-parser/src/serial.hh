/*
 * \file serial.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <string>
#include <termios.h>

typedef struct{
    std::string port;
    speed_t baudrate;
}SerialConfig_t;

class SerialInterface
{
    int m_serial;

    int setAttributes(speed_t baudrate);

public:
    SerialInterface(const SerialConfig_t sConfig);
    ~SerialInterface();

    std::string getNextLine();
};
