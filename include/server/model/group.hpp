//
// Created by turing on 24-7-30.
//

#ifndef GROUP_HPP
#define GROUP_HPP
#include <muduo/net/TcpConnection.h>
#include <bits/stdc++.h>
#include <json.hpp>
#include "groupuser.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

class Group {
public:
    Group(int id = -1, string name = "", string desc = ""): id(id), name(name), desc(desc) {
    }

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setDesc(string desc) { this->desc = desc; }

    int getId() { return this->id; }
    string getName() { return this->name; }
    string getDesc() { return this->desc; }
    vector<GroupUser> &getUsers() { return this->users; }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};

#endif //GROUP_HPP
