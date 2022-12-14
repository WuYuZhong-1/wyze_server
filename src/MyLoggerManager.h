#ifndef MYLOGGERMANAGER_H
#define MYLOGGERMANAGER_H
#define SPDLOG_ACTIVE_LEVEL   SPDLOG_LEVEL_DEBUG        //此宏必须先定义
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

/*************************************************
    后续如果不需要spdlog,可以通过修改此类，避免内部改动
**************************************************/


//spd 带行号打印，同时输出console 和 文件
#define DEBUG(...)  do { SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_DEBUG(spdlog::get("daily"), __VA_ARGS__); }while(0)
#define INFO(...) do { SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_INFO(spdlog::get("daily"), __VA_ARGS__); }while(0)
#define WARN(...) do { SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_WARN(spdlog::get("daily"), __VA_ARGS__); }while(0)
#define ERROR(...) do { SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_ERROR(spdlog::get("daily"), __VA_ARGS__); }while(0)
#define CRITICAL(...) do { SPDLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_CRITICAL(spdlog::get("daily"), __VA_ARGS__); }while(0)

extern bool log_init;

#define _DEBUG(...) do { if(log_init) { DEBUG(__VA_ARGS__); } else { SPDLOG_DEBUG(__VA_ARGS__); } } while(0)
#define _INFO(...) do { if(log_init) { INFO(__VA_ARGS__); } else { SPDLOG_INFO(__VA_ARGS__); } } while(0)
#define _WARN(...) do { if(log_init) { WARN(__VA_ARGS__); } else { SPDLOG_WARN(__VA_ARGS__); } } while(0)
#define _ERROR(...) do { if(log_init) { ERROR(__VA_ARGS__); } else { SPDLOG_ERROR(__VA_ARGS__); } } while(0)
#define _CRITICAL(...) do { if(log_init) { CRITICAL(__VA_ARGS__); } else { SPDLOG_CRITICAL(__VA_ARGS__); } } while(0)

namespace wyze {

    class MyLoggerManager {

    public:
        MyLoggerManager();
        ~MyLoggerManager();

    private:
        std::shared_ptr<spdlog::logger> m_daily_logger;
        std::shared_ptr<spdlog::logger> m_console;

    };


}

#endif // MYLOGGERMANAGER_H