project(pc-parser)
cmake_minimum_required(VERSION 2.6)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT COMPONENTS Core Graf)
find_package(Catch2 REQUIRED)

include(${ROOT_USE_FILE})

add_custom_command(
    OUTPUT cmdline.c
    COMMAND gengetopt --input=${PROJECT_SOURCE_DIR}/src/command-line.ggo --output-dir=${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS src/command-line.ggo
)

set(SOURCE_FILES
    src/serial.cc
    src/eeg-graph.cc
    src/wave-generator.cc
    src/eeg-data-decoding.cc
    ${CMAKE_CURRENT_BINARY_DIR}/cmdline.c
)

set(TEST_SOURCE_FILES
    tests/rolling-array-tests.cc
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(pc-parser src/main.cc ${SOURCE_FILES})
add_executable(tests ${TEST_SOURCE_FILES})

target_link_libraries(pc-parser ${ROOT_LIBRARIES})
target_link_libraries(pc-parser m)
target_link_libraries(pc-parser fftw3)

target_link_libraries(tests Catch2::Catch2)
