/*
 * \file array.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <mutex>

template<class T, int SIZE>
class Array
{
    unsigned int m_index;
    int m_size;
    T m_data[SIZE * 2];
    std::mutex m_mutex;

public:
    Array() :
    m_index(0),
    m_size(0)
    {
        for (int i = 0; i < SIZE * 2; i++) {
            m_data[i] = 0;
        }
    }

    inline void append(T item)
    {
        m_mutex.lock();

        if (m_size < SIZE) {
            m_data[m_size] = item;
            m_data[SIZE + m_size++] = item;
        } else {
            m_index++;
            m_data[m_index - 1] = item;
            m_data[m_index + SIZE - 1] = item;

            if (m_index >= SIZE) {
                m_index -= SIZE;
            }
        }

        m_mutex.unlock();
    }

    inline int getSize()
    {
        return m_size;
    }

    inline T* getData()
    {
        return m_data + m_index;
    }

    inline void lock()
    {
        m_mutex.lock();
    }

    inline void unlock()
    {
        m_mutex.unlock();
    }

    /*
    inline T* getActualData()
    {
        return m_data;
    }

    inline unsigned int getActualIndex()
    {
        return m_index;
    }
    */
};
