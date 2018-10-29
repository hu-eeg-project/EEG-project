/*
 * \file serial.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <string>

class SerialInterface
{
    int m_serial;

    int setAttributes();

public:
    SerialInterface(const std::string fname);
    ~SerialInterface();

    std::string getNextLine();
};
