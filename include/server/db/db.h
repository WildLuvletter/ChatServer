//
// Created by turing on 24-7-29.
//

#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>
#include <chatservice.hpp>
#include <public.hpp>
#include <muduo/base/Logging.h>
#include "Pool.h"

// 数据库操作类
class MySQL {
public:
    // 初始化数据库连接
    MySQL();

    // 释放数据库连接资源~MySQL()
    ~MySQL();

    // 连接数据库
    bool connect();

    // 更新操作
    bool update(string sql);

    // 查询操作
    MYSQL_RES *query(string sql);

    //获取链接
    MYSQL* getConnection();

private:
    MYSQL *_conn;
};

#endif //DB_H
