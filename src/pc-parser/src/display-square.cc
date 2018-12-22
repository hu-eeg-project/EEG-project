/*
 * \file display-square.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "display-square.hh"

DisplaySquare::DisplaySquare(bool text, std::string path_to_ttf) :
m_box(),
m_circle(0, 100),
m_shape(nullptr),
m_has_flashed(false),
m_initial_flash(true),
m_text(text)
{
    m_box.setPosition(sf::Vector2f(560, 140));
    m_box.setSize(sf::Vector2f(800, 800));
    m_box.setFillColor(sf::Color::Yellow);

    m_circle.setPosition(sf::Vector2f(560, 140));
    m_circle.setRadius(400);
    m_circle.setFillColor(sf::Color::Blue);

    if (!m_font.loadFromFile(path_to_ttf)) {
        printf("Couldn't load font!\n");
        exit(1);
    }

    if (m_text) {
        m_text1.setFont(m_font);
        m_text1.setCharacterSize(500);
        m_text1.setString(std::to_string((std::rand() % 5 * 2) + 1));
        m_text1.setFillColor(sf::Color::White);
        m_text1.setPosition(sf::Vector2f(710, 290));

        m_text2.setFont(m_font);
        m_text2.setCharacterSize(500);
        m_text2.setString(std::to_string(std::rand() % 5 * 2));
        m_text2.setFillColor(sf::Color::White);
        m_text2.setPosition(sf::Vector2f(710, 290));
    }
}

bool DisplaySquare::update()
{
    m_window.clear(sf::Color::Black);

    sf::Time elapsed = m_clock.getElapsedTime();
    if (m_initial_flash) {
        if (elapsed.asSeconds() > 12.0f) {
            m_clock = sf::Clock();
            m_initial_flash = false;
        }
    } else if (elapsed.asSeconds() > 0.5f) {
        m_flash = false;
        m_clock = sf::Clock();
    } else if (elapsed.asSeconds() > 0.4f) {
        if (!m_flash) {
            if (std::rand() % 10 == 0) {
                if (m_text) m_shape2 = &m_text1;
                else m_shape = &m_box;
            } else {
                if (m_text) m_shape2 = &m_text2;
                else m_shape = &m_circle;
            }
        }
        m_flash = true;
        m_has_flashed = true;

        if (m_text) m_window.draw(*m_shape2);
        else m_window.draw(*m_shape);
    }

    return Display::update();
}

record_data_t DisplaySquare::recording()
{
    sf::Time elapsed = m_clock.getElapsedTime();

    if (elapsed.asSeconds() >= 0.8f &&
        elapsed.asSeconds() < 1.0f) {
        if (m_text) {
            if (m_shape2 == &m_text1) return {true, "square"};
            else return {true, "circle"};
        } else {
            if (m_shape == &m_box) return {true, "square"};
            else return {true, "circle"};
        }
    } else if (elapsed.asSeconds() > 0.1f &&
               elapsed.asSeconds() < 0.5f && m_has_flashed) {
        if (m_text) {
            if (m_shape2 == &m_text1) return {true, "square"};
            else return {true, "circle"};
        } else {
            if (m_shape == &m_box) return {true, "square"};
            else return {true, "circle"};
        }
    }

    return {false, ""};
}
