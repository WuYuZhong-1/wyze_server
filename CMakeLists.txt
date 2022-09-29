cmake_minimum_required(VERSION 2.8)
project(wyze)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGES "-O0 -std=c++11 -Wall -Werror")

link_directories(
    ${PROJECT_SOURCE_DIR}/depend/lib
    )
include_directories(
    ${PROJECT_SOURCE_DIR}/depend/include
    ${PROJECT_SOURCE_DIR}/src
    )


set(SRC 
    src/MyLoggerManager.cpp
    src/MyUtil.cpp
    src/MyConfig.cpp
    src/MysqlConn.cpp
    src/MysqlConnPool.cpp
    src/MyThreadPool.cpp
    src/MySocketEvent.cpp
    src/MyTcpServerEvent.cpp
    src/MyBufferEvent.cpp
    src/main.cpp
    )

set(TEST
    src/MyLoggerManager.cpp
    src/MyUtil.cpp
    src/MyConfig.cpp
    test/test.cpp
    )

add_executable(test ${TEST} )
add_dependencies(test yaml-cpp spdlog mysqlclient pthread event)
target_link_libraries(test yaml-cpp spdlog mysqlclient pthread event)

add_executable(main ${SRC})
add_dependencies(main yaml-cpp spdlog mysqlclient pthread event)
target_link_libraries(main yaml-cpp spdlog mysqlclient pthread event)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)