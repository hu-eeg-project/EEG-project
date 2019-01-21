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

/**
 * \brief A struct representing a physical monitor.
 */
typedef struct monitor {
    int id; /*!< The Id of the monitor */
    int width; /*!< Width of the monitor */
    int height; /*!< Height of the monitor */
    int x; /*!< X offset of monitor */
    int y; /*!< Y offset of monitor */
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
}

Display::~Display()
{

}

bool Display::update()
{
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
