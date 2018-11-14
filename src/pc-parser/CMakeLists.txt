project(pc-parser)
cmake_minimum_required(VERSION 2.6)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT COMPONENTS Core Graf)

include(${ROOT_USE_FILE})

set(SOURCE_FILES
    src/main.cc
    src/serial.cc
    src/eeg-graph.cc
    src/wave-generator.cc
    src/eeg-data-decoding.cc
)

#include_directories("/usr/include/root/")

add_executable(pc-parser ${SOURCE_FILES})
target_link_libraries(pc-parser ${ROOT_LIBRARIES})
target_link_libraries(pc-parser m)
target_link_libraries(pc-parser fftw3)
