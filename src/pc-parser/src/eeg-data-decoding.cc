#include "eeg-data-decoding.hh"
#include <iostream>
#include <sstream>

void loopDecodeNonBatched(SerialInterface& sf,
                          ArrayPair<RollingArray<int16_t>, RollingArray<Double_t>>* array)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;
    st = std::chrono::high_resolution_clock::now();
    while(true){
        std::string input = sf.getNextLine();
        if(input.size()){
            std::stringstream stream(input);
            char type, delim;
            int16_t eeg_data;
            stream >> type >> delim >> eeg_data;
            if(type == 'd'){
                printf("%i\n", eeg_data);
                nt = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> dt = nt-st;
                array->lock();
                array->array1.append(eeg_data);
                array->array2.append(dt.count());
                array->unlock();
            }
        }
    }
}

void loopDecodeBatched(SerialInterface& sf,
                       ArrayPair<RollingArray<int16_t>, RollingArray<Double_t>>* array,
                       const size_t frame_size)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;
    st = std::chrono::high_resolution_clock::now();
    
    size_t frame_index = 0;
    int16_t * frame = new int16_t[frame_size];
    int32_t frame_dur = 0;
    double last = 0;
    while(true) {
        std::string input = sf.getNextLine();
        uint point_dur;
        if(input.size()){
            std::stringstream stream(input);
            char type, delim;
            int16_t eeg_data;
            stream >> type >> delim >> eeg_data >> delim >> point_dur;
            if(type == 'd'){
                frame[frame_index] = eeg_data;
                frame_index+=1;
                frame_dur += point_dur;
                if(frame_index >= frame_size){
                    nt = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> dt = nt-st;
                    double end_time = dt.count() + frame_dur;
                    std::cout<<frame_dur<<std::endl;
                    double avrg_frame_time = (double)frame_dur / frame_size / 1000000;
                    array->lock();
                    double current = 0;
                    for(int i = 0;i<frame_index;i++){
                        array->array1.append(frame[i]);
                        current = dt.count() + avrg_frame_time * i;
                        if(current<last){
                            //std::cout<<last<<' '<<current<<std::endl;
                        }
                        last = current;
                        array->array2.append(dt.count() + avrg_frame_time * i);
                    }
                    array->unlock();
                    
                    frame_index = 0;
                    frame_dur = 0;
                }
            }
        }
    }
}
