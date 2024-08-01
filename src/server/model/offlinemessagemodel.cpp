//
// Created by turing on 24-7-29.
//
#include"offlinemessagemodel.hpp"
#include "db.h"
#include "Connect.h"

void OfflineMsgModel::insert(int userid, string msg) {
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values('%d', '%s')", userid, msg.c_str());
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

vector<string> OfflineMsgModel::query(int userid) {
    vector<string> ans;
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid=%d", userid);
    // MySQL mysql;
    shared_ptr<Connect> mysql = Pool::getPool()->getConnect();
    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            ans.push_back(row[0]);
        }
        mysql_free_result(res);
    }

    return ans;
}

void OfflineMsgModel::remove(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", userid);
    // MySQL mysql;
    shared_ptr<Connect> mysql = Pool::getPool()->getConnect();
    mysql->update(sql);
}
