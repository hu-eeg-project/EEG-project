/*
 * \file main.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */

#include <stdio.h>
#include <unistd.h>

#include "serial.hh"
#include "eeg-graph.hh"
#include "array.hh"

#include <thread>

void dataThread(ArrayPair<Array<int, 10>, Array<int, 10>>* array)
{
    for (int i = 0; i < 10000; i++) {
        array->lock();
        array->array1.append(i);
        array->array2.append(10000 - i);
        array->unlock();
        //usleep(1000 * 20);
    }
}

int main(int argc, char* argv[])
{
    Array<int, 10> data_array;
    Array<int, 10> time_array;
    ArrayPair<Array<int, 10>, Array<int, 10>> data(data_array, time_array);

    std::thread data_thread(dataThread, &data);

    bool looping = true;
    while (looping) {
        printf("Array:\n");
        data.lock();
        for (int i = 0; i < data.array1.getSize(); i++) {
            printf("[%4i]", data.array1.getData()[i]);

            if (data.array1.getData()[i] == 9999) looping = false;
        }
        printf("\n");
        for (int i = 0; i < data.array2.getSize(); i++) {
            printf("[%4i]", data.array2.getData()[i]);
        }
        data.unlock();
        printf("\n\n");
    }

    data_thread.join();

    /*
    EEGGraph eeg(&argc, argv);

    while (true)
    {
        eeg.update();

        usleep(1000*16);
    }
    */

    /*
    SerialInterface sf("/dev/ttyUSB0");

    while (true) {
        printf("Line: %s\n", sf.getNextLine().c_str());
    }
    */

    return 0;
}
