/*
 * \file display.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

class Display
{
    sf::Event event;

protected:
    sf::RenderWindow m_window;
    double m_time;
    sf::Clock m_clock;
    bool m_flash;
    int m_flash_counter;
    double m_flash_stop;

public:
    Display();
    ~Display();

    virtual bool update();
    bool recording();
};
