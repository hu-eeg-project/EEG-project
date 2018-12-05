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
m_window(sf::VideoMode(1920, 1080), "P300 Display", sf::Style::None)
{
    m_window.clear();

    m_window.display();

    std::istringstream res =
        std::istringstream(exec("xrandr | grep ' connected' | tr ' ' '\n' | grep '+'"));

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

    m_time = 3 + std::rand() % 10;

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

void Display::update()
{
    sf::Time elapsed = m_clock.getElapsedTime();

    if (elapsed.asSeconds() > m_time && elapsed.asSeconds() < m_time + 0.05f) {
        m_window.clear(sf::Color::White);
        m_text.setFillColor(sf::Color::Black);
    } else {
        m_window.clear(sf::Color::Black);
        m_text.setFillColor(sf::Color::White);
    }

    for (char c = 'A'; c <= 'Z'; c++){
        m_text.setString(std::string(1, c));
        m_text.setPosition(((c - 'A') % 8) * 160.0f + 160.0f, ((c - 'A') / 8) * 160.0f + 160.0f);
        m_window.draw(m_text);
    }

    m_window.display();
}

bool Display::recording()
{
    sf::Time elapsed = m_clock.getElapsedTime();

    if (elapsed.asSeconds() >= m_time && elapsed.asSeconds() < m_time + 0.8f) {
        return true;
    } else if (elapsed.asSeconds() >= m_time + 0.2f) {
        m_clock = sf::Clock();
        m_time = 3 + std::rand() % 5;
    }

    return false;
}