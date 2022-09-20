cmake_minimum_required(VERSION 2.8)
project(wyze)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGES "$ENV(CXXFLAGS) -rdynamic -O0 -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(SRC 
    src/main.cpp 
    )

set(TEST
    test/test.cpp
    )

add_executable(test ${TEST})
link_directories(${PROJECT_SOURCE_DIR}/depend/yaml-cpp/lib)
include_directories(${PROJECT_SOURCE_DIR}/depend/yaml-cpp/include)
add_dependencies(test yaml-cpp)
target_link_libraries(test yaml-cpp)

add_executable(main ${SRC})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)