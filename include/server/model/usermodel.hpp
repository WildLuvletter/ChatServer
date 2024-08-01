//
// Created by turing on 24-7-29.
//

#ifndef USERMODEL_HPP
#define USERMODEL_HPP
#include<bits/stdc++.h>
#include"user.hpp"
using namespace std;

//User表的数据操作类
class UserModel {
public:
    bool insert(User &user);

    User query(int id);

    bool updateState(User& user);

    //重置用户的状态信息
    void resetState();

private:
};

#endif //USERMODEL_HPP
