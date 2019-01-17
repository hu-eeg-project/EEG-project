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

/**
 * \brief The configuration of the serial port
 */
typedef struct{
    std::string port; /*!< Path to the port to listen on */
    speed_t baudrate; /*!< The baudrate of the serial port */
} SerialConfig_t;

/**
 * \brief An interface for serial communication
 */
class SerialInterface
{
    int m_serial;

    int setAttributes(speed_t baudrate);

public:
    /**
     * \brief Creates a serial interface with the given configuration
     * \param sConfig The configuration to use
     */
    SerialInterface(const SerialConfig_t sConfig);
    ~SerialInterface();

    /**
     * \brief Gets the next line that was recieved on the serial port
     * \return A string which contains the text of the next line from the serial port
     */
    std::string getNextLine();
};
