/*
 * \file display-square.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include "display.hh"

#define DEFAULT_FONT "/usr/share/fonts/noto/NotoSans-Regular.ttf"

class DisplaySquare : public Display
{
    sf::Font m_font;
    sf::Text m_text1;
    sf::Text m_text2;

    sf::RectangleShape m_box;
    sf::CircleShape m_circle;
    sf::Shape* m_shape;
    sf::Text* m_shape2;
    bool m_has_flashed;
    bool m_initial_flash;
    bool m_text;

public:
    DisplaySquare(bool text = false, std::string path_to_ttf = DEFAULT_FONT);

    bool update() override;
    record_data_t recording() override;
};
