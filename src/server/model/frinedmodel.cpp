//
// Created by turing on 24-7-29.
//
#include "friendmodel.hpp"
#include "db.h"
#include "Connect.h"

void FriendModel::insert(int userid, int friendid) {
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);
    // MySQL mysql;
    shared_ptr<Connect> mysql = Pool::getPool()->getConnect();
    mysql->update(sql);
}

vector<User> FriendModel::query(int userid) {
    vector<User> ans;
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.name,a.state from user a join friend b on a.id=b.friendid where b.userid=%d", userid);
    // MySQL mysql;
    shared_ptr<Connect> mysql = Pool::getPool()->getConnect();
    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            ans.push_back(User(atoi(row[0]), row[1], "", row[2]));
        }
        mysql_free_result(res);
    }

    return ans;
}
