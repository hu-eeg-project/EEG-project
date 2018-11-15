/*
 * \file array.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <mutex>

template<class A, class B>
class ArrayPair
{
    std::mutex m_mutex;

public:
    A& array1;
    B& array2;

    ArrayPair(A& a1, B& a2) :
    array1(a1),
    array2(a2)
    {

    }

    inline void lock()
    {
        m_mutex.lock();
    }

    inline void unlock()
    {
        m_mutex.unlock();
    }
};

template<class T>
class RollingArray
{
    unsigned int m_index;
    int m_size;
    T * m_data;
    std::mutex m_mutex;
    size_t m_length;

public:
    RollingArray(size_t length):
    m_index(0),
    m_size(0),
    m_length(length)
    {
	m_data = new T[length * 2];
    }

    ~RollingArray(){
	delete m_data;
    }
    
    inline void append(T item)
    {
        m_mutex.lock();

        if (m_size < m_length) {
            m_data[m_size] = item;
            m_data[m_length + m_size++] = item;
        } else {
            m_index++;
            m_data[m_index - 1] = item;
            m_data[m_index + m_length - 1] = item;

            if (m_index >= m_length) {
                m_index -= m_length;
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

    inline T operator[](size_t index){
        T* array_begin = getData();
        return array_begin[index];
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
