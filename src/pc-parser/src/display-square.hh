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

/**
 * \brief The display class for a square + circle
 */
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
    /**
     * \brief Creates the display for the square + circle
     * \param text If true uses two random numbers if false shows a square or circle
     * \param path_to_ttf The path to the TTF font to use for the font rendering
     */
    DisplaySquare(bool text = false, std::string path_to_ttf = DEFAULT_FONT);

    bool update() override;
    record_data_t recording() override;
};
