#include "MyLoggerManager.h"
#include <iostream>


namespace wyze {

    MyLoggerManager::MyLoggerManager() 
    {
        try {
            std::string file_pattern = "[%H-%m-%d %H:%M:%S] [%S:%#(%!)] [%n] [%l] [%thread:%t] : %v";
            //创建循环日志
            // auto rotating_logger = spdlog::rotating_logger_mt("rotating","rotating_log.txt", 1024*1024*4, 3);

            //创建每日日志，在 2.30am 更新
            m_daily_logger = spdlog::daily_logger_mt("daily", "logs/daily_log.txt", 2, 30);
            m_daily_logger->set_pattern(file_pattern);
            m_daily_logger->set_level(spdlog::level::info);

            //设置遇到 warn以上的日志刷新
            m_daily_logger->flush_on(spdlog::level::warn);

            //每三秒刷新一次
            spdlog::flush_every(std::chrono::seconds(3));

            //控制台输出
            std::string stdout_pattern = "[%H-%m-%d %H:%M:%S] [%S:%#(%!)] [%n] [%^%l%$] [thread:%t] : %v";
            m_console = spdlog::stdout_color_mt("console");
            m_console->set_pattern(stdout_pattern);
            m_console->set_level(spdlog::level::debug);


            //设置默认输出
            spdlog::set_default_logger(m_console);

        }
        catch(const spdlog::spdlog_ex& ex) {
            std::cout << "spdlog init error:" << ex.what() << std::endl;
        }
    }

    MyLoggerManager::~MyLoggerManager() {
        spdlog::drop_all();
    }


}