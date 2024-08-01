//
// Created by turing on 24-7-29.
//

#ifndef OFFLINEMESSAGEMODEL_HPP
#define OFFLINEMESSAGEMODEL_HPP

#include <muduo/net/TcpConnection.h>
#include <bits/stdc++.h>
#include <json.hpp>
#include "usermodel.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;
//提供离线消息表的操作接口
class OfflineMsgModel {
public:
    //存储用户的离线消息
    void insert(int userid,string msg);
    //删除用户的离线消息
    void remove(int userid);
    //查询用户的离线消息
    vector<string>query(int userid);
private:
};

#endif //OFFLINEMESSAGEMODEL_HPP
