//
// Created by turing on 24-7-30.
//

#ifndef GROUPMODEL_HPP
#define GROUPMODEL_HPP
#include "group.hpp"
#include <bits/stdc++.h>
using namespace std;

class GroupModel {
public:
    //创建群组
    bool createGroup(Group &group);

    //加入群组
    void addGroup(int userid, int groupid, string role);

    //查询用户所在群组信息
    vector<Group> queryGroups(int userid);

    //根据指定的groupid查询群组用户id列表,出userid自己,主要用户群聊业务给其他群组成员发消息
    vector<int> queryGroupUsers(int userid, int groupid);

private:
};

#endif //GROUPMODEL_HPP
