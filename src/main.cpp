#include <iostream>
#include "MyLoggerManager.h"

// using namespace wyze;

int main(int argc, char** argv)
{
    wyze::MyLoggerManager manager;

    DEBUG("debug");
    INFO("info");
    // WARN("warn");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}