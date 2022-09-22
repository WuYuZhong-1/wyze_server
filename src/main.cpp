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
    wyze::MySingle<wyze::MyConfigManager>::getInstance()->loadYaml(default_server_config);

    std::function<void(int, int)> f1 = 
        [](const int new_value,const int old_new) {
            _INFO("new_value: {}, old_new: {}", new_value, old_new);
        };

    wyze::MyConfigCallBack<int> fun1(1, f1);

    auto base_var =
    wyze::MySingle<wyze::MyConfigManager>::getInstance()->lookup(CONFIG_UTIL_VAR1);
    auto int_var = std::dynamic_pointer_cast<wyze::MyConfigVar<int>>(base_var);
    int_var->addListener(fun1);

    int_var->setValue(100);
    
    wyze::MyLoggerManager manager;

    DEBUG("debug");
    INFO("info");
    // WARN("warn");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}