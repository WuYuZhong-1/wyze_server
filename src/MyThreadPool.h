#ifndef _MY_THREAD_POOL_H_
#define _MY_THREAD_POOL_H_

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>

namespace wyze {

    class MyThreadPool {
    public:
        MyThreadPool(size_t);
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) 
            -> std::future<typename std::result_of<F(Args...)>::type>;
        ~MyThreadPool();
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
    };

    inline MyThreadPool::MyThreadPool(size_t threads)
        :   stop(false)
    {
        for(size_t i = 0; i < threads; ++i) 
        {
            workers.emplace_back(
                [this] 
                {
                    for(;;) 
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock, 
                                [this]{ return this->stop || !this->tasks.empty(); });
                            if(this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                }
            );
        }
    }

    template<class F, class... Args>
    auto MyThreadPool::enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        //将任务打包
        auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)  
        );

        //获取future
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            if(stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace([task](){ (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    inline MyThreadPool::~MyThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(auto& worker: workers) 
            worker.join();
    }
}



#endif //_MY_THREAD_POOL_H_