//
// Created by turing on 24-6-8.
//

#include "Connect.h"

Connect::Connect() {
    _conn = mysql_init(nullptr);
}

Connect::~Connect() {
    if (_conn != nullptr)
        mysql_close(_conn);
}

bool Connect::connect(string ip, unsigned short port, string username, string password, string dbname) {
    MYSQL *p = mysql_real_connect(_conn, ip.c_str(), username.c_str(), password.c_str(),
                                  dbname.c_str(), port, nullptr, 0);
    return p!=nullptr;
}

bool Connect::update(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                << sql << "更新失败!";
        return false;
    }
    return true;
}

MYSQL_RES *Connect::query(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                << sql << "查询失败!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}
MYSQL *Connect::getConnection() {
    return this->_conn;
}

