/*
 * \file array.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <mutex>

/**
 * \brief Pairs two arrays with one mutex to lock both
 * \tparam A The first array
 * \tparam B The second array
 */
template<class A, class B>
class ArrayPair
{
    std::mutex m_mutex;

public:
    A& array1; /*!< First array of the pair */
    B& array2; /*!< Second array of the pair */

    /**
     * \brief Creates an array pair
     * \param a1 The first array
     * \param a2 The second array
     */
    ArrayPair(A& a1, B& a2) :
    array1(a1),
    array2(a2)
    {

    }

    /**
     * \brief Locks the arrays for use
     */
    inline void lock()
    {
        m_mutex.lock();
    }

    /**
     * \brief Unlocks the arrays again
     */
    inline void unlock()
    {
        m_mutex.unlock();
    }
};

/**
 * \brief Creates a rolling array where when the size is hit it will remove the oldest value and append the new value
 * \tparam T The data type to create a rolling array off
 */
template<class T>
class RollingArray
{
    unsigned int m_index;
    int m_size;
    T * m_data;
    std::mutex m_mutex;
    size_t m_length;

public:
    /**
     * \brief Creates a rolling array
     * \param length The ammount of data that fits in the array
     */
    RollingArray(size_t length):
    m_index(0),
    m_size(0),
    m_length(length)
    {
	m_data = new T[length * 2];
    }

    ~RollingArray(){
	delete[] m_data;
    }

    /**
     * \brief Appends an item to the array and removes the oldest one if the array is at it's max size
     * \param item The item to add
     */
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

    /**
     * Gets the ammount of items in the array
     */
    inline int getSize()
    {
        return m_size;
    }

    /**
     * \brief Gets a pointer to the data points in the array as a normal array (ordered from old to newest added)
     * \return The pointer to the data
     */
    inline T* getData()
    {
        return m_data + m_index;
    }

    /**
     * \brief Locks the mutex of the array
     */
    inline void lock()
    {
        m_mutex.lock();
    }

    /**
     * \brief Unlocks the mutex of the array
     */
    inline void unlock()
    {
        m_mutex.unlock();
    }

    /**
     * \brief Gets an item at a certain index from the array
     * \param index The index to get the item from
     * \return The item at the given index
     */
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
