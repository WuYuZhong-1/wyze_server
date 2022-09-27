#include "MyLoggerManager.h"
#include <iostream>
#include "MyConfig.h"

bool log_init = false;

namespace wyze {

    MyLoggerManager::MyLoggerManager() 
    {
        try {

            auto confman = MySinglePtr<MyConfigManager>::getInstance();
            auto base_var = confman->lookup(CONFIG_UTIL_LOGGER);
            if(base_var) {
                auto var = std::dynamic_pointer_cast<MyConfigVar<std::set<StLogger>>>(base_var);
                if(var){
                    _INFO("logger: {}", var->toString());

                    for(auto& it : var->getValue()) {
                        std::string pattern = it.pattern;
                        std::shared_ptr<spdlog::logger> log;
                        if(it.append == EnAppendType::daily) {
                            log = spdlog::daily_logger_format_mt(it.name, it.file, it.para.daily.hour, it.para.daily.min);
                        }
                        else if(it.append == EnAppendType::rotating) {
                            log = spdlog::rotating_logger_mt(it.name, it.file, it.para.rotating.filesize, it.para.rotating.filenum);
                        }
                        else if(it.append == EnAppendType::stdout) {
                            log = spdlog::stdout_color_mt(it.name);
                            spdlog::set_default_logger(log);
                        }
                        log->set_level(static_cast<spdlog::level::level_enum>(it.level));
                        log->set_pattern(it.pattern);
                    }

                }
            }

            // std::string file_pattern = "[%H-%m-%d %H:%M:%S] [%s:%#(%!)] [%n] [%l] [%thread:%t] : %v";
            // //创建循环日志
            // // auto rotating_logger = spdlog::rotating_logger_mt("rotating","rotating_log.txt", 1024*1024*4, 3);

            // //创建每日日志，在 2.30am 更新
            // m_daily_logger = spdlog::daily_logger_mt("daily", "logs/daily_log.txt", 2, 30);
            // m_daily_logger->set_pattern(file_pattern);
            // m_daily_logger->set_level(spdlog::level::info);

            // //设置遇到 warn以上的日志刷新
            // m_daily_logger->flush_on(spdlog::level::warn);

            // //每三秒刷新一次
            // spdlog::flush_every(std::chrono::seconds(3));

            // //控制台输出
            // std::string stdout_pattern = "[%H-%m-%d %H:%M:%S] [%s:%#(%!)] [%n] [%^%l%$] [thread:%t] : %v";
            // m_console = spdlog::stdout_color_mt("console");
            // m_console->set_pattern(stdout_pattern);
            // m_console->set_level(spdlog::level::debug);


            // //设置默认输出
            // spdlog::set_default_logger(m_console);

            //设置全局变量log_init 初始化，这时所有的 输出信息使用初始化后的 打印，否则使用初始化前的打印
            log_init = true;
        }
        catch(const spdlog::spdlog_ex& ex) {
            _ERROR("spdlog init error:{}", ex.what());
        }
    }

    MyLoggerManager::~MyLoggerManager() {
        spdlog::drop_all();
    }


}