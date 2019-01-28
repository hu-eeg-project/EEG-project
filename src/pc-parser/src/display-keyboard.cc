/*
 * \file display-keyboard.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "display-keyboard.hh"

DisplayKeyboard::DisplayKeyboard(double m_time, size_t size, std::string path_to_ttf):
    m_size(size),
    m_random_c('A'),
    m_time(m_time)
{
    if (!m_font.loadFromFile(path_to_ttf)) {
        printf("Couldn't load font!\n");
        exit(1);
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(160);
}

bool DisplayKeyboard::update()
{
    sf::Time elapsed = m_clock.getElapsedTime();
    if (elapsed.asSeconds() > m_time && !is_recording) {
        //m_time = elapsed.asSeconds();
        m_random_c = 'A' + std::rand() % m_size;
        is_recording = true;
    }

    for (char c = 'A'; c <= 'A' + m_size; c++){
        m_text.setString(std::string(1, c));
        m_text.setPosition(((c - 'A') % 6) * 160.0f + 160.0f,
                           ((c - 'A') / 6) * 160.0f + 160.0f);

        if(c == m_random_c && elapsed.asSeconds() < 0.6f){       
            m_text.setFillColor(sf::Color::White);
        }
        else m_text.setFillColor(sf::Color(50, 50, 50));
        
        m_window.draw(m_text);
    }
    return Display::update();
}

record_data_t DisplayKeyboard::recording()
{
    sf::Time elapsed = m_clock.getElapsedTime();

    if (is_recording &&
        elapsed.asSeconds() > m_time + 0.01f) {
        m_clock = sf::Clock();
        is_recording = false;
    }
    else if (is_recording &&
        elapsed.asSeconds() > m_time + 0.2f){
        return {true, std::string(1, m_random_c)};
    }     
    return {false, ""};
}
