#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <memory>
#include "MyUtil.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "MyLoggerManager.h"
#include "yaml-cpp/yaml.h"
#include <sstream>
#include <set>



#define CONFIG_UTIL_LOGGER         "logger"
#define CONFIG_UTIL_VAR2         "wuyze2"
#define CONFIG_UTIL_VAR3         "wuyze3"
#define CONFIG_UTIL_VAR4         "wuyze4"

//重载less 片特滑
// namespace std {

//     class wyze::StLogger;
//     template<>
//     struct less<> : public binary_function<wyze::StLogger, wyze::StLogger, bool>
//     {
//       _GLIBCXX14_CONSTEXPR
//       bool
//       operator()(const wyze::StLogger& __x, const wyze::StLogger& __y) const
//       { return __x < __y; }
//     };

// }

namespace wyze {

    template<class From, class To>
    class MyLexicalCast {
    public:
        To operator()(const From& value) {
            try{
                return boost::lexical_cast<To>(value);
            }
            catch(const boost::bad_lexical_cast& ex ) {
                // std::cout << "lexical_cast error:" << ex.what() << std::endl;
                _ERROR("lexical_cast error:{}", ex.what());
                throw;
            }
            
        }
    };


    template<class T>
    class MyLexicalCast<std::string, std::set<T>> {
    public:
        std::set<T> operator()(const std::string &value) {
            
            YAML::Node root = YAML::Load(value);
            
            std::set<T> contain;
            std::stringstream ss;

            for(int i = 0; i < root.size(); ++i) {
                ss.str("");
                ss << root[i];
                contain.insert(MyLexicalCast<std::string, T>()(ss.str()));
            }
            return contain;
        }
    };

    template<class T>
    class MyLexicalCast<std::set<T>, std::string> {
    public:
        std::string operator()(const std::set<T>& v) {
            std::stringstream ss;
            for(auto& i : v) {
                ss << MyLexicalCast<T, std::string>()(i);
            }
            return ss.str();
        }
    };

    /***************************************配置变量类只函数数据， 所以使用 struct 定义***********************************************************/
    enum EnAppendType { unknow = 0, stdout = 1, daily, rotating };

    struct StDaily {
        uint32_t hour = 0;              //daily 时 表示 每日文件更新小时点
        uint32_t min  = 0;               //daily 时 表示 每日更新分钟点

        bool operator==(const StDaily& other) const {
            if( hour == other.hour
                && min == other.min)
                return true;
            return false;
        }
    };

    struct StRotating {
        uint32_t filesize;          //rotating 时 表示文件大小
        uint32_t filenum;           //rotationg 时 表示文件数目

        bool operator==(const StRotating& other) const {
            if(filesize == other.filesize
                && filenum == other.filenum)
                return true;
            return false;
        }
    };

    //log类
    struct StLogger {
        std::string name;      
        std::string pattern;
        uint32_t level;
        EnAppendType append;                //当 append 为 daily，rotating时，才会有下面的参数
        std::string file;                   //文件名
        union parameter {
            parameter() : daily() {}
            StDaily daily;
            StRotating rotating;
        } para;

        bool operator<(const StLogger& other) const {
            return name < other.name;
        }
        
        bool operator==(const StLogger& other)  const {
            if(name == other.name && pattern == other.pattern && level == other.level
                && append == other.append && file == other.file && para.daily == other.para.daily )
                return true;
            return false;
        }

        //TODO::这里可能还需要自己实现 复制构造和 赋值构造，
        //但是编译器已经自己实现，先看看编译器实现的能不能用
    };


    #define LOGGER_NAME "name"
    #define LOGGER_PATTERN "pattern"
    #define LOGGER_LEVEL "level"
    #define LOGGER_APPEND "append"
    #define LOGGER_FILE "file"
    #define LOGGER_FIRSTPARAMETER "firstparameter"
    #define LOGGER_SECONDPARAMETER "secondparameter"

    //偏特化 MyLexicalCast
    template<>
    class MyLexicalCast<std::string, StLogger> {
    public:
        StLogger operator()(const std::string& val) {
            YAML::Node root = YAML::Load(val);
            StLogger logger;
            logger.name = root[LOGGER_NAME].as<std::string>();
            logger.pattern = root[LOGGER_PATTERN].as<std::string>();
            logger.level = root[LOGGER_LEVEL].as<int>();

            {
                int append = root[LOGGER_APPEND].as<int>();
                if(append <= EnAppendType::unknow 
                    || append > EnAppendType::rotating)
                    logger.append = EnAppendType::stdout;
                else
                    logger.append = static_cast<EnAppendType>(append);
            }
            
            if(logger.append != EnAppendType::stdout) {
                logger.file = root[LOGGER_FILE].as<std::string>();
                logger.para.daily.hour = root[LOGGER_FIRSTPARAMETER].as<int>();
                logger.para.daily.min = root[LOGGER_SECONDPARAMETER].as<int>();

            }
            
            return logger;
        }
    };

    template<>
    class MyLexicalCast<StLogger, std::string> {
    public:
        std::string operator()(const StLogger& val) {

            std::stringstream ss;
            ss << "[ ";
            ss << "name:\"" << val.name << "\", "
                << "pattern:\"" << val.pattern << "\", "
                << "level:\"" << val.level << "\", "
                << "append:\"" << val.append ;

            if(val.append == EnAppendType::daily ){
                ss << "\", " << "file:\""<<val.file 
                    << "\", " << "hour:\""<< val.para.daily.hour
                    << "\", " << "min:\"" << val.para.daily.min << " ]";
            }
            else if(val.append == EnAppendType::rotating) {
                ss << "\", " << "file:\"" << val.file
                    << "\", " << "filesize:\"" << val.para.rotating.filesize
                    << "\", " << "filenum:\"" << val.para.rotating.filenum << "\" ]";
            }
            else {
                ss << "\" ]";
            }

            return ss.str();
        }
    };
    



    //配置文件变量类
    class MyConfigVarBase {
    public:
        typedef std::shared_ptr<MyConfigVarBase> ptr;

        //set集合需要用到的比较
        struct less {
            bool operator()(const ptr& a, const ptr& b) {
                return a->m_name < b->m_name;
            }
        };

        MyConfigVarBase(const std::string& name, const std::string& description)
            : m_name(name), m_description(description) { };
        virtual ~MyConfigVarBase() { }

        const std::string& getName() const { return m_name; }
        const std::string& getDescription() const { return m_description; }

        virtual std::string toString() = 0;
        virtual bool fromString(const std::string& val) = 0;
        virtual std::string getTypeName() const = 0 ;

    private:
        std::string m_name;
        std::string m_description;
    };

    //模板类配置文件变量，可以为 内嵌类型变量，也可以是自定意义变量
    template<class T, class FromStr = MyLexicalCast<std::string, T>, class ToStr = MyLexicalCast<T, std::string>>
    class MyConfigVar: public MyConfigVarBase {
    public:
        typedef std::shared_ptr<MyConfigVar> ptr;

        //TODO::  构造函数中，需要用到模板 T 对象的复制构造函数
        MyConfigVar(const std::string& name, const T& value, const std::string& desc)
            : MyConfigVarBase(name, desc), m_value(value) { }
        
        //TODO::这里用到模板 T 对象赋值构造函数
        const T getValue() const { return m_value; }
        //TODO::这里用到 == 运算符, 且 = 运算符
        void setValue(const T& value) {
            if( m_value == value)
                return;
            
            try{
                //出发监听变量的函数
                for(auto i : m_callback_set) {
                    i(this->getName(), value, m_value);
                }
                m_value = value;
            }
            catch(...) {
                // std::cout << "[" << __FILE__ << ":" << __LINE__ << "] setValue exception";
                _ERROR("setValue exception");
            }
        }

        void addListener(MyConfigCallBack<T> cb) { m_callback_set.insert(cb); }
        void delListener(MyConfigCallBack<T> cb) { m_callback_set.erase(cb); }
        void clearListener() { m_callback_set.clear(); }

        /************实现父类的虚函数****************/
        std::string toString() override {
            try {
                //return boost::lexical_cast<std::string>(m_val);
                return ToStr()(m_value);
            }
            catch( std::exception& e) {    
                // std::cout << "ConfigVar::toString() exception:" << e.what()
                //                                 << " convert: " << typeid(m_value).name() << " to std::string" << std::endl;
                _ERROR("ConfigVar::toString() exception:{} convert: {} to std::string", e.what(), typeid(m_value).name());
            }
            return "";
        }

        bool fromString(const std::string& value) override {
            try {
                setValue(FromStr()(value));
                return true;
            }
            catch(const std::exception& ex)  {
                // std::cout << "ConfigVar::fromString() exception:" << ex.what()
                //                             << " convert:  std::string to " << typeid(m_value).name()
                //                             << " - " << value << std::endl;
                _ERROR("ConfigVar::fromString() exception:{} convert: std::string to {}", ex.what(), typeid(m_value).name());
            }
            return false;
        }

        std::string getTypeName() const override { return typeid(T).name(); }

    private:
        T m_value;
        std::set<MyConfigCallBack<T>, typename MyConfigCallBack<T>::less> m_callback_set;
    };

    class MyConfigManager {
    public:
        MyConfigManager() {
            m_configvar_set.clear();
        };

        //加载配置文件，
        void loadYaml(const std::string& file) {

            if(file.empty()) 
                return;

            try {
                YAML::Node root = YAML::LoadFile(file);   
                init_logger(root);
                

            }
            catch(...) {
                _ERROR("loadYaml ERROR");
            }
        }

        //查找配置变量
        typename MyConfigVarBase::ptr lookup(const std::string& name) {
            for(auto it : m_configvar_set) {
                if(it->getName() == name){
                    //TODO::这里可以，需要修改为指定的配置变量类型
                    return it;
                }
            }
            return nullptr;
        }

        //删除配置变量
        bool delConfigVar(const std::string& name) {
            for(auto it = m_configvar_set.begin(); 
                it != m_configvar_set.end(); ++it) {

                if((*it)->getName() == name) {
                    m_configvar_set.erase(it);
                    return true;
                }
            }
            return false;
        }

        //添加配置变量
        auto addConfigVar(const MyConfigVarBase::ptr var) {
            return m_configvar_set.insert(var);
        }

    private:

        //logger 配置文件的加载
        void init_logger(const YAML::Node& root) {

            auto value = root[CONFIG_UTIL_LOGGER];
            std::stringstream ss ;
            ss << value;
            auto var = lookup(CONFIG_UTIL_LOGGER);
            if(var) {
                //找到该变量
                auto logger_var = std::dynamic_pointer_cast<MyConfigVar<std::set<StLogger>>>(var);
                if(logger_var) {
                    //转换成功
                    logger_var->fromString(ss.str());
                }
                else {
                    //这里报错
                    _ERROR("init_logger error, typeid is {}", var->getTypeName());
                }
            }
            else {
                //未找到
                MyConfigVar<std::set<StLogger>>::ptr v(new MyConfigVar<std::set<StLogger>>(CONFIG_UTIL_LOGGER,std::set<StLogger>(), CONFIG_UTIL_LOGGER));
                if(v->fromString(ss.str())) {
                    addConfigVar( v);
                }             
            }
        }

    private:
        std::set<MyConfigVarBase::ptr, typename MyConfigVarBase::less> m_configvar_set;
    };

}

#endif // !MYCONFIG_H
