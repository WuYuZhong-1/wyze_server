#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <memory>
#include "MyUtil.h"
#include <set>
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace wuyze {
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

        //重载 == 

        
        MyConfigVarBase(const std::string& name, const std::string& description)
            : m_name(name), m_description(description) { };
        virtual ~MyConfigVarBase() { }

        const std::string& getName() const { return m_name; }
        const std::string& getDescription() const { return m_description; }

        virtual std::string toString() = 0;
        virtual bool fromString(const std::string& val) = 0;
        virtual std::string getTypeName() = 0 ;

    private:
        std::string m_name;
        std::string m_description;
    };

    template<class From, class To>
    class LexicalCast {
        To operator()(const From& value) {
            try{
                return boost::lexical_cast<From>(value);
            }
            catch(const boost::bad_lexical_cast& ex ) {
                std::cout << "lexical_cast error:" << ex.what() << std::endl;
                throw;
            }
            
        }
    };


    //模板类配置文件变量，可以为 内嵌类型变量，也可以是自定意义变量
    template<class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
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
                    (*i)(value, m_value);
                }
                m_value = value;
            }
            catch(...) {
                std::cout << "[" << __FILE__ << ":" << __LINE__ << "] setValue exception";
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
                std::cout << "ConfigVar::toString() exception:" << e.what()
                                                << " convert: " << typeid(m_value).name() << " to std::string" << std::endl;
            }
            return "";
        }

        bool fromString(const std::string& value) override {
            try {
                setValue(FromStr()(value));
                return true;
            }
            catch(const std::exception& ex)  {
                std::cout << "ConfigVar::fromString() exception:" << ex.what()
                                            << " convert:  std::string to " << typeid(value).name()
                                            << " - " << value << std::endl;
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
        MyConfigManager(std::string& file): m_config_file(file) {
            m_configvar_set.clear();
        };

        //加载配置文件，
        void loadYaml() {

        }

        //查找配置变量
        template<class T>
        typename MyConfigVar<T>::ptr lookup(const std::string& name) {
            for(auto it : m_configvar_set) {
                if(it->getName() == name){
                    //删除操作
                    return std::dynamic_pointer_cast<MyConfigVar<T>>(it);
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

    private:
        std::string m_config_file;   //配置文件名
        std::set<MyConfigVarBase::ptr, typename MyConfigVarBase::less> m_configvar_set;
    };

}

#endif // !MYCONFIG_H
