/*
 * \file display.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "display.hh"

#include <list>
#include <sstream>
#include <stdio.h>
#include <random>

typedef struct monitor {
    int id;
    int width;
    int height;
    int x;
    int y;
} monitor_t;

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

Display::Display() :
m_window(sf::VideoMode(1920, 1080), "P300 Display", sf::Style::None),
m_flash(false),
m_flash_counter(0),
m_flash_stop(0)
{
    m_window.clear();

    m_window.display();

    std::istringstream res =
        std::istringstream(exec("xrandr | grep ' connected' "
                                "| tr ' ' '\n' | grep '+'"));

    std::list<monitor_t> monitors;
    std::string line;
    int id = 1;
    while (std::getline(res, line)) {
        int width, height, x, y;
        sscanf(line.c_str(), "%ix%i+%i+%i", &width, &height, &x, &y);

        monitor_t m = {id++, width, height, x, y};
        monitors.push_back(m);
    }

    int current_monitor = -1;
    for (monitor_t& i : monitors) {
        int x = m_window.getPosition().x;
        int y = m_window.getPosition().y;
        if (i.x <= x && i.x + i.width > x && i.y <= y && i.y + i.height > y) {
            current_monitor = i.id;
            break;
        }
    }
    for (monitor_t& i : monitors) {
        if (i.id != current_monitor) {
            m_window.setPosition(sf::Vector2i(i.x, i.y));
            m_window.setSize(sf::Vector2u(i.width, i.height));

            m_window.requestFocus();
        }
    }

    m_clock = sf::Clock();

    //m_time = 3 + std::rand() % 10;
    m_time = 1;
    
    if (!m_font.loadFromFile("/usr/share/fonts/noto/NotoSans-Regular.ttf")) {
        printf("Couldn't load font!\n");
        exit(1);
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(160);
}

Display::~Display()
{

}

bool Display::update()
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
        
        //m_window.clear(sf::Color::White);
        //m_text.setFillColor(sf::Color::Black);
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
        //m_window.clear(sf::Color::Black);
        //m_text.setFillColor(sf::Color::White);
    } else {
        m_flash = false;
        //m_window.clear(sf::Color::Black);
        //m_text.setFillColor(sf::Color::White);
    }

    for (char c = 'A'; c <= 'Z'; c++){
        m_text.setString(std::string(1, c));
        m_text.setPosition(((c - 'A') % 6) * 160.0f + 160.0f,
                           ((c - 'A') / 6) * 160.0f + 160.0f);
        if(c == random_c) m_text.setFillColor(sf::Color::White);
        else m_text.setFillColor(sf::Color(50, 50, 50));
        m_window.draw(m_text);
    }

    while(m_window.pollEvent(event)){
        if(event.type == sf::Event::Closed) return false; // Close program.
        if(event.type == sf::Event::KeyPressed){
            if(event.key.code == sf::Keyboard::Escape){
                m_window.close();
                return false; // Close program.
            }
        }
    }
    m_window.display();
    return true; // Program schould not close.
}

bool Display::recording()
{
    sf::Time elapsed = m_clock.getElapsedTime();

    if (elapsed.asSeconds() >= m_time &&
        elapsed.asSeconds() < m_time + 1.8f &&
        m_flash_counter > 5) {
        return true;
    } else if (elapsed.asSeconds() >= m_time + 1.2f) {
        m_clock = sf::Clock();
        m_time = 3 + std::rand() % 5;
        m_flash_counter = 0;
    }

    return false;
}
