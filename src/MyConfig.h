#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <memory>
#include "MyUtil.h"
#include <set>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "MyLoggerManager.h"
#include "yaml-cpp/yaml.h"


#define CONFIG_UTIL_VAR1         "wuyze1"
#define CONFIG_UTIL_VAR2         "wuyze2"
#define CONFIG_UTIL_VAR3         "wuyze3"
#define CONFIG_UTIL_VAR4         "wuyze4"




namespace wyze {
    using namespace wyze;
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
        //TODO::这里用到 == 运算符
        const void setValue(const T& value) {
            if( value == m_value)
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

                
                m_configvar_set.insert(MyConfigVar<int>::ptr(
                    new MyConfigVar<int>(CONFIG_UTIL_VAR1, root[CONFIG_UTIL_VAR1].as<int>(),CONFIG_UTIL_VAR1)
                ));
                
                m_configvar_set.insert(MyConfigVar<int>::ptr(
                    new MyConfigVar<int>(CONFIG_UTIL_VAR2, root[CONFIG_UTIL_VAR2].as<int>(), CONFIG_UTIL_VAR2)
                ));

                m_configvar_set.insert(MyConfigVar<int>::ptr(
                    new MyConfigVar<int>(CONFIG_UTIL_VAR3, root[CONFIG_UTIL_VAR3].as<int>(), CONFIG_UTIL_VAR3)
                ));

                m_configvar_set.insert(MyConfigVar<int>::ptr(
                    new MyConfigVar<int>(CONFIG_UTIL_VAR4, root[CONFIG_UTIL_VAR4].as<int>(), CONFIG_UTIL_VAR4)
                ));

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
        std::set<MyConfigVarBase::ptr, typename MyConfigVarBase::less> m_configvar_set;
    };

}

#endif // !MYCONFIG_H
