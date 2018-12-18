/*
 * \file display-square.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "display-square.hh"

DisplaySquare::DisplaySquare() :
m_box(),
m_circle(0, 100),
m_shape(nullptr),
m_has_flashed(false)
{
    m_box.setPosition(sf::Vector2f(560, 140));
    m_box.setSize(sf::Vector2f(800, 800));
    m_box.setFillColor(sf::Color::Red);

    m_circle.setPosition(sf::Vector2f(560, 140));
    m_circle.setRadius(400);
    m_circle.setFillColor(sf::Color::Blue);
}

bool DisplaySquare::update()
{
    m_window.clear(sf::Color::Black);

    sf::Time elapsed = m_clock.getElapsedTime();
    if (elapsed.asSeconds() > 0.5f) {
        m_flash = false;
        m_clock = sf::Clock();
    } else if (elapsed.asSeconds() > 0.4f) {
        if (!m_flash) {
            if (std::rand() % 10 == 0) {
                m_shape = &m_box;
            } else {
                m_shape = &m_circle;
            }
        }
        m_flash = true;
        m_has_flashed = true;

        m_window.draw(*m_shape);
    }

    return Display::update();
}

record_data_t DisplaySquare::recording()
{
    sf::Time elapsed = m_clock.getElapsedTime();

    if (elapsed.asSeconds() >= 0.6f &&
        elapsed.asSeconds() < 1.0f) {
        if (m_shape == &m_box) return {true, "square"};
        else return {true, "circle"};
    } else if (elapsed.asSeconds() > 0.1f &&
               elapsed.asSeconds() < 0.5f && m_has_flashed) {
        if (m_shape == &m_box) return {true, "square"};
        else return {true, "circle"};
    }

    return {false, ""};
}
