#ifndef _MYSQLCONN_H_
#define _MYSQLCONN_H_

#include <string>
#include <mysql/mysql.h>
#include <chrono>
#include <iostream>

namespace wyze {

    #define logger      std::cout       //替换，此类在别处单独拿出来还可以使用

    class MysqlConn {
    public:
        //初始化数据库连接
        MysqlConn();
        //释放数据库连接
        ~MysqlConn();
        //连接数据库
        bool connect(const std::string& user, const std::string& password, const std::string& dbName,
                        const std::string& ip, unsigned short port = 3306);
        //更新数据库 insert, update, delete
        bool update(const std::string& sql);
        //查询数据库
        bool query(const std::string& sql);
        //遍历得到的结果集合
        bool next();
        //得到结果集合中的字段值
        std::string value(int index);
        //字段的个数
        int fields();
        //事物操作
        bool transaction();
        //事物提交
        bool commit();
        //事物回滚
        bool rollback();
        //刷新空闲时间点
        void refreshAliveTime();
        //计算连接存活的总时长
        long long getAliveTime();
    private:
        void freeResult();

    private:
        MYSQL* m_mysql = nullptr;
        MYSQL_RES* m_result = nullptr;
        MYSQL_ROW m_row = nullptr;
        std::chrono::steady_clock::time_point  m_aliveTime;
    };

}

#endif // _MYSQLCONN_H_