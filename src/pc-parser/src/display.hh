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
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    double m_time;

public:
    Display();
    ~Display();

    void update();
    bool recording();
};
