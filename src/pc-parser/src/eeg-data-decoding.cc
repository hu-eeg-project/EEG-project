#include "eeg-data-decoding.hh"
#include <iostream>
#include <sstream>

#define ERROR_UNKOWN_CMD   0x01
#define ERROR_BAD_SIGNAL   0x02
#define ERROR_BAD_CHECKSUM 0x03
#define ERROR_PACKET_LEN   0x04

std::string getStringForError(int error_code)
{
    switch (error_code)
    {
        case ERROR_UNKOWN_CMD:
            return "Unknown CMD";
        case ERROR_BAD_SIGNAL:
            return "Bad Signal";
        case ERROR_BAD_CHECKSUM:
            return "Bad Checksum";
        case ERROR_PACKET_LEN:
            return "Packet Length";
        default:
            break;
    }

    return "Unknown Error Code";
}

void loopDecodeNonBatched(SerialInterface& sf,
                          ArrayPair<RollingArray<Double_t>, RollingArray<Double_t>>* array)
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
                //printf("%i\n", eeg_data);
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
                       ArrayPair<RollingArray<Double_t>,
                       RollingArray<Double_t>>* array,
                       const size_t frame_size)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> st =
        std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt;
    double frame_start_time;

    int number_of_points;
    int current_point = 0;
    int frame_duration;
    double point_increment;

    while (true)
    {
        std::string input = sf.getNextLine();

        if (input.size()) {
            std::stringstream input_stream(input);
            char type, delim;

            input_stream >> type >> delim;

            switch (type)
            {
                case 'f':
                    input_stream >> number_of_points >> delim >> frame_duration;

                    dt = std::chrono::high_resolution_clock::now() - st;
                    frame_start_time = dt.count() - frame_duration / 1000000;
                    point_increment = frame_duration / 1000000.0 / number_of_points;

                    current_point = 0;

                    break;

                case 'd':
                    int16_t eeg_data;
                    input_stream >> eeg_data;

                    array->lock();
                    array->array1.append(eeg_data);
                    array->array2.append(frame_start_time +
                                         point_increment * current_point++);
                    array->unlock();

                    break;

                case 'a': // Attention
                case 'h': // Heart Rate
                case 'm': // Meditation
                case 'w': // Waves
                    break;

                case 'b': // Batch specifier
                    break;

                case 'r':
                    int error_code;
                    input_stream >> error_code;

                    break;

                default:
                    std::cout << "Unknown command type: " << type << std::endl;
                    break;
            }
        }
    }
}
