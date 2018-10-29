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

void dataThread(Array<int, 10>* array)
{
    for (int i = 0; i < 100; i++) {
        array->append(i);
        usleep(1000 * 20);
    }
}

int main(int argc, char* argv[])
{
    Array<int, 10> data;

    std::thread data_thread(dataThread, &data);

    bool looping = true;
    while (looping) {
        printf("Array:\n");
        data.lock();
        for (int i = 0; i < data.getSize(); i++) {
            printf("[%2i]", data.getData()[i]);

            if (data.getData()[i] == 99) looping = false;
        }
        data.unlock();
        printf("\n");
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
