/*
 * \file display-keyboard.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include "display.hh"

class DisplayKeyboard : public Display
{
    sf::Font m_font;
    sf::Text m_text;

public:
    DisplayKeyboard();

    bool update() override;
};
