/*
 * \file display.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

/**
 * \brief Used to determine if data is being recorded or not to a .eeg file
 */
typedef struct
{
    bool recording; /*!< If true it is recording to a file. */
    std::string metadata; /*!< Extra metadata to save in the recording file. */
} record_data_t;

/**
 * \brief The base Display class
 */
class Display
{
    sf::Event event;

protected:
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    bool m_flash;
    int m_flash_counter;
    double m_flash_stop;

public:
    Display();
    ~Display();

    /**
     * \brief Called to update the display and render the next frame
     */
    virtual bool update();

    /**
     * \brief called to check wether the data needs to be currently recorded or not
     * \return The data structure that says wether to record or not with metadata
     */
    virtual record_data_t recording() = 0;

};
