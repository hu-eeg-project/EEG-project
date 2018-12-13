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
    sf::Font m_font;
    sf::Text m_text;
    sf::Event event;
    bool m_flash;
    int m_flash_counter;
    double m_flash_stop;

public:
    Display();
    ~Display();

    bool update();
    bool recording();
    
};
