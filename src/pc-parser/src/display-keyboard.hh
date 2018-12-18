/*
 * \file display-keyboard.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include "display.hh"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#define DEFAULT_FONT "/usr/share/fonts/noto/NotoSans-Regular.ttf"

class DisplayKeyboard : public Display
{
    sf::Font m_font;
    sf::Text m_text;
    double m_time;
    size_t m_size;
    char m_random_c;
    bool is_recording = false;

public:
    DisplayKeyboard(double time, size_t size=26, std::string path_to_ttf = DEFAULT_FONT);

    bool update() override;
    record_data_t recording() override;
};
