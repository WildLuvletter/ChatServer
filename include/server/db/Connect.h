//
// Created by turing on 24-6-8.
//

#ifndef CONNECT_H
#define CONNECT_H

#include "mysql/mysql.h"
#include "bits/stdc++.h"
#include <muduo/base/Logging.h>
using namespace std;

class Connect {
public:
    Connect();

    ~Connect();

    bool connect(string ip, unsigned short port, string username, string password, string dbname);

    bool update(string sql);

    MYSQL_RES *query(string sql);

    void refreshAliveTime() { _alivetime = clock(); }
    clock_t getAliveTime() const { return clock() - _alivetime; }
    //获取链接
    MYSQL* getConnection();

private:
    MYSQL *_conn; //a mysql.txt connection
    clock_t _alivetime;
};


#endif //CONNECT_H
