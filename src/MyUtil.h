#ifndef MYUTIL_H
#define MYUTIL_H


/****************************************
 *  一些对工程有用的 小功能
 ****************************************/
#include <functional>
#include <memory>

namespace wyze {

    //配置变量中，变量的值发生改变会出发监听函数(回调)，为了增删查改 std::function
    // 该类指向一个 std::function 和 有一个标志变量

    template<class T>
    class MyConfigCallBack {
    public:

        //set 集合需要用到比较
        struct less {
            bool operator()(const MyConfigCallBack<T>& a, const MyConfigCallBack<T>& b) const {
                return a.m_flag < b.m_flag;
            }
        };

        typedef std::function<void(const std::string& name,const T& new_value, const T& old_value)> CallBack;
        MyConfigCallBack(uint64_t flag, const CallBack& fun)
            : m_fun(fun), m_flag(flag) { }
        
        void operator()(const std::string& name, const T& new_value, const T& old_value) {
            m_fun(name, new_value, old_value);
        }

        bool operator == (const MyConfigCallBack& other) {
            return m_flag == other.m_flag ? true : false;
        }
    
    private:
        CallBack m_fun;
        uint64_t m_flag;
    } ;



    //单例模式
    template<class T>
    class MySingle {
    public:
        static T* getInstance() {
            static T value;
            return &value;
        }
    };

    //单例模式，智能指针
    template<class T>
    class MySinglePtr {
    public:
        static std::shared_ptr<T> getInstance() {
            static std::shared_ptr<T> value(new T);
            return value;
        }
    };
}

#endif // !MYUTIL_H