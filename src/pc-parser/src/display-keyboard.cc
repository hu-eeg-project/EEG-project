/*
 * \file display-keyboard.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "display-keyboard.hh"

DisplayKeyboard::DisplayKeyboard()
{
    if (!m_font.loadFromFile("/usr/share/fonts/noto/NotoSans-Regular.ttf")) {
        printf("Couldn't load font!\n");
        exit(1);
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(160);
}

bool DisplayKeyboard::update()
{
    sf::Time elapsed = m_clock.getElapsedTime();
    char random_c = 'A' + rand() % 26;
    if (elapsed.asSeconds() > m_time && m_flash_counter == 0) {
        m_flash_stop = elapsed.asSeconds() + 0.05f;
    }

    if (elapsed.asSeconds() > m_time &&
        elapsed.asSeconds() < m_flash_stop &&
        m_flash_counter <= 5) {

        //char random_c = 'A' + rand() % 26;

        if (!m_flash) {
            m_flash = true;
            m_flash_counter++;
        }
    } else if (m_flash_counter <= 5) {
        if (m_flash) {
            m_time = m_flash_stop + 0.3f;
            m_flash_stop = m_time + 0.05f;
            m_flash = false;
        }
    } else {
        m_flash = false;
    }

    for (char c = 'A'; c <= 'Z'; c++){
        m_text.setString(std::string(1, c));
        m_text.setPosition(((c - 'A') % 6) * 160.0f + 160.0f,
                           ((c - 'A') / 6) * 160.0f + 160.0f);
        if(c == random_c) m_text.setFillColor(sf::Color::White);
        else m_text.setFillColor(sf::Color(50, 50, 50));
        m_window.draw(m_text);
    }

    return Display::update();
}
