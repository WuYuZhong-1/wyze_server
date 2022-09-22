#include <iostream>
#include "yaml-cpp/yaml.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include <thread>
#include <vector>

//后续增加 spdlog 会进行替换
#define Cout   std::cout 


class TestYaml {
public:
    void operator()() {
        try{
            YAML::Node config = YAML::LoadFile("../test/config.yaml");

            Cout << "name = " << config["name"].as<std::string>() << std::endl;
            Cout << "sex = " << config["sex"].as<std::string>() << std::endl;
            Cout << "age = " << config["age"].as<int>() << std::endl;

            Cout << "c++ = " << config["skill"]["c++"].as<int>() << std::endl;
            for(auto it =  config["skill"].begin() ;
                    it != config["skill"].end(); ++it) {
                Cout << it->first << " = " << it->second << std::endl;
            }

            std::string str1 = "[1, 2, 3, 4]";
            YAML::Node node1 = YAML::Load(str1);
            Cout << str1 << " type is " << type_to_string(node1.Type()) << std::endl;

            std::string str2 = "wuyze";
            YAML::Node node2 = YAML::Load(str2);
            Cout << str2 << " type is " << type_to_string(node2.Type()) << std::endl;

            std::string str3 = "{name: wyze, id: 1}";
            YAML::Node node3 = YAML::Load(str3);
            Cout << str3 << " type is " << type_to_string(node3.Type()) << std::endl;
        }
        catch(...) {
            Cout << "error\n" ;
        }
    }

private:
    std::string type_to_string(YAML::NodeType::value value) {
        switch(value) {
            case YAML::NodeType::Undefined:
                return {"Undefined"}; 
            case YAML::NodeType::Null:
                return {"Null"};
            case YAML::NodeType::Scalar:
                return {"Scalar"};
            case YAML::NodeType::Sequence:
                return {"Sequence"};
            case YAML::NodeType::Map:
                return {"Map"};
        }
        return "";
    }
};



class TestSpdlog{
public:
    void operator() () {

        //直接输出字符串,全局模板参数，函数内部默认创建一个logger
        spdlog::info("welcome to spdlog");
        //参数化
        spdlog::error("some error message with arg: {}", 1);
        //高级参数
        spdlog::warn("easy padding in numbers like {:08d}", 12);
        //特殊参数，多种格式
        //spdlog::critical("support for int {0:d}; hex:{0:x}", 42);
        spdlog::critical("Support for int:{0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
        //浮点数
        spdlog::info("support for floats {:03.2f}", 1.23456);
        //定制顺序
        spdlog::info("positional args art {1} {0}", "too", "supported");
        //排版格式
        spdlog::info("{:>80}", "left aligned");


        //创建一个自己的对象
        auto console = spdlog::stdout_color_mt("console");
        console->info("my console logger");
        console->info("{}", console->level());
        console->set_pattern("[%Y-%m-d %H:%M:%S] [%n] [%^%l%$] [thread:%t]: %v");
        console->info("change pattern");
        spdlog::stderr_color_mt("stderr");
        spdlog::get("stderr")->info("stderr");

        //基础文件日志
        try {
            auto logger = spdlog::basic_logger_mt("basic_logger", "./logs/basic-log.txt");
            logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] [thread:%t] : %v");
            logger->info("hello wyze");
        }
        catch(const spdlog::spdlog_ex &ex) {
            std::cout << "Log init failed: " << ex.what() << std::endl;
        }
#if 0
        //循环日志
        try{
            auto max_size = 128;
            auto max_files = 3;
            auto logger = spdlog::rotating_logger_mt("rotating", "./logs/rotating-log.txt", 
                                                max_size, max_files);
            logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%l] [thread:%t] : %v");

            int count = 10;
            while(count--) {
                std::string message('a', 100);
                console->info("test rotating");
                logger->info(message);
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
        catch(const spdlog::spdlog_ex &ex) {
            std::cout << "rotating logger error:" << ex.what() << std::endl;
        }

        //每日日志
        try{
            auto logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);
            logger->set_pattern("%Y-%m-%d %H:%M:%S] [%n] [%l] [thread:%t] : %v");
            logger->info("daily log");
        }
        catch(const spdlog::spdlog_ex &ex)  {
            std::cout << "daily logger error: " << ex.what() << std::endl;
        }

        //异步日志
        try {
            auto async_logger = spdlog::daily_logger_mt<spdlog::async_factory>("async_logger", "logs/async_daily.txt", 2, 30);
            async_logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%l] [thread:%t] : %v");
            int count = 10;
            while(count--) {
                console->info("test async");
                async_logger->info("async");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }

        }
        catch(const spdlog::spdlog_ex &ex) {
            std::cout << "async logger error" << ex.what() << std::endl;
        }

#endif
        //创建复合logger
        try {
            std::string pattern1 = "[%Y-%m-%d %H:%M:%S] [%@] [%n] [%l] [thread:%t] : %v";
            auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/multi_async_daily.txt", 2, 30);
            file_sink->set_level(spdlog::level::warn);
            file_sink->set_pattern(pattern1);

            std::string pattern2 = "[%Y-%m-%d %H:%M:%S] [%@] [%n] [%^%l%$] [thread:%t] : %v";
            auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            stdout_sink->set_level(spdlog::level::debug);
            stdout_sink->set_pattern(pattern2);


            std::vector<spdlog::sink_ptr> sinks;
            sinks.push_back(file_sink);
            sinks.push_back(stdout_sink);
            
            auto logger = std::make_shared<spdlog::logger>("multi_sink", begin(sinks), end(sinks));

            spdlog::set_default_logger(logger);

            int count = 3;
            while(count--) {
                logger->info("info multi logger");
                std::this_thread::sleep_for(std::chrono::seconds(1));
                logger->warn("warn multi logger");
                std::this_thread::sleep_for(std::chrono::seconds(1));
                SPDLOG_ERROR("ERROR");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

        }
        catch(const spdlog::spdlog_ex &ex) {
            std::cout << "mutil logger error: " << ex.what() << std::endl;
        }

        //宏打印
        SPDLOG_INFO("FILE AND LINE");

    }

};


#include <boost/lexical_cast.hpp>

class MyLexicalCast {
public:
    void operator() () {

        std::string str_int = std::to_string((int)10);
        std::cout << "str_int = " << str_int << std::endl;
        std::string str_float = std::to_string((float)10.0);
        std::cout << "str_float = " << str_float << std::endl;
        std::string str_double = std::to_string((double)12.555); 
        std::cout << "str_double = " << str_double << std::endl;

        int i = boost::lexical_cast<int>(std::string("199"));
        std::cout << "i = " << i << std::endl;
        double d = boost::lexical_cast<double>(std::string("12.3456789").c_str(), 4);
        std::cout << "d = " << d << std::endl;

        std::string str = boost::lexical_cast<std::string>(100);
    }

};



//只能指针放入set集合是如何比较
#include <set>

class A {
public:
    class A_less {
    public:
        bool operator()(const std::shared_ptr<A>& a, const std::shared_ptr<A>& b) const {
            std::cout << "a:" << a->m_i << std::endl;
            std::cout << "b:" << b->m_i << std::endl;
            return a->m_i < b->m_i;
        }
    };

    bool operator == (const A& other) {
        std::cout << "self:" << m_i << std::endl;
        std::cout << "other:" << other.m_i << std::endl;
        return m_i == other.m_i;
    }

    A(int i, int j) : m_i(i), m_j(j){
        std::cout << "ctor" << std::endl;
    };
    ~A() {
        std::cout << "dtor" << std::endl;
    }
    std::string print() const {
        return "{" + std::to_string(m_i) + "," + std::to_string(m_j) + "}";
    }

public:
    int m_i;
    int m_j;
};




std::ostream& operator<<(std::ostream& os, const A& a) {
    os << a.print();
    return os;
}

class shared_set {

public:
    void operator()() {

        std::set<std::shared_ptr<A>, A::A_less> set;
        std::shared_ptr<A> a1(new A(10,10));
        set.insert(a1);
        set.insert(std::make_shared<A>(20,10));
        set.insert(std::make_shared<A>(30, 30));
        set.insert(std::make_shared<A>(10,10));

        for(auto it: set) {
            std::cout << *it << std::endl;
        }

        auto it = set.find(a1);
        if(it == set.end()) {
            std::cout << "don't find"<< std::endl;
        }
        else {
            std::cout << *(*it) << std::endl;
        }

        //需要增加比较函数

    }
};


int main(int argc, char** argv)
{
    // TestYaml()();
    // TestSpdlog()();
    // MyLexicalCast()();
    shared_set()();
    return 0;
}