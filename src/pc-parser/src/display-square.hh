/*
 * \file display-square.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include "display.hh"

class DisplaySquare : public Display
{
    sf::RectangleShape m_box;
    sf::CircleShape m_circle;
    sf::Shape* m_shape;
    bool m_has_flashed;

public:
    DisplaySquare();

    bool update() override;
    record_data_t recording() override;
};
