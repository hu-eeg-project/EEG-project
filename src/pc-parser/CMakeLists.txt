project(pc-parser)
cmake_minimum_required(VERSION 2.6)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT COMPONENTS Core Graf)
find_package(Catch2 REQUIRED)

include(${ROOT_USE_FILE})

set(SOURCE_FILES
    src/serial.cc
    src/eeg-graph.cc
    src/wave-generator.cc
    src/eeg-data-decoding.cc
)

set(TEST_SOURCE_FILES
    tests/rolling-array-tests.cc    
)

#include_directories("/usr/include/root/")

add_executable(pc-parser src/main.cc ${SOURCE_FILES})
add_executable(tests ${TEST_SOURCE_FILES})

target_link_libraries(pc-parser ${ROOT_LIBRARIES})
target_link_libraries(pc-parser m)
target_link_libraries(pc-parser fftw3)

target_link_libraries(tests Catch2::Catch2)
