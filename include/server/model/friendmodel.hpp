//
// Created by turing on 24-7-29.
//

#ifndef FRIENDMODEL_HPP
#define FRIENDMODEL_HPP
#include <muduo/net/TcpConnection.h>
#include <bits/stdc++.h>
#include <json.hpp>
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

//维护好友信息的操作接口
class FriendModel {
public:
    //添加好友关系
    void insert(int userid,int friendid);

    //返回用户好友列表
    vector<User>query(int userid);
};

#endif //FRIENDMODEL_HPP
