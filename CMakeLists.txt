cmake_minimum_required(VERSION 2.8)
project(wyze)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGES "-O0 -std=c++11 -Wall -Werror")

link_directories(
    ${PROJECT_SOURCE_DIR}/depend/lib
    )
include_directories(
    ${PROJECT_SOURCE_DIR}/depend/include
    )


set(SRC 
    src/main.cpp 
    )

set(TEST
    test/test.cpp
    )

add_executable(test ${TEST})
add_dependencies(test yaml-cpp spdlog)
target_link_libraries(test yaml-cpp spdlog)

add_executable(main ${SRC})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)