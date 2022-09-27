#include <iostream>
#include "MyLoggerManager.h"
#include "MyUtil.h"
#include "MyConfig.h"

// using namespace wyze;

static std::string default_server_config =  "../config/server_config.yaml";

int main(int argc, char** argv)
{
    //当运行程序，不传入配置文件路径，则会使用默认的配置文件路径进行配置
    if(argc >= 2) {
        default_server_config = argv[1];
    }
    //初始化配置管理类
    auto confman = wyze::MySinglePtr<wyze::MyConfigManager>::getInstance();
    confman->loadYaml(default_server_config);

    //初始化 spdlog，配置相关 log
    wyze::MyLoggerManager manager;

    DEBUG("debug");
    INFO("info");
    // WARN("warn");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}