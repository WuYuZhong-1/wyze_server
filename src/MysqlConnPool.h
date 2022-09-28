#ifndef _MYSQLCONNPOOL_H_
#define _MYSQLCONNPOOL_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "MysqlConn.h"
#include <thread>

namespace wyze {
    
    class MysqlConnPool {
    public:
        static MysqlConnPool* getInstance();
        std::shared_ptr<MysqlConn> getConnection();
        ~MysqlConnPool();

    private:
        MysqlConnPool();
        MysqlConnPool(const MysqlConnPool& pool) = delete;
        MysqlConnPool(const MysqlConnPool&& pool) = delete;
        MysqlConnPool& operator=(const MysqlConnPool& pool) = delete;
        MysqlConnPool& operator=(const MysqlConnPool&& pool) = delete;
        bool parseConfig();
        void produceConnection();
        void recycleConnection();
        void addConnection();
    private:
        std::string m_ip;
        unsigned short m_port;
        std::string m_user;
        std::string m_password;
        std::string m_dbName;
        int m_minSize;
        int m_maxSize;
        int m_timeout;
        int m_maxIdleTime;
        std::queue<MysqlConn*> m_mysqlConnQueue;
        std::mutex m_mutexQ;
        std::condition_variable m_getconn;
        std::condition_variable m_produce;

        std::shared_ptr<std::thread> m_producer;
        std::shared_ptr<std::thread> m_recycler;
        bool m_isexit = false;

    };
}


#endif //_MYSQLCONNPOOL_H_