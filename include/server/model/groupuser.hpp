//
// Created by turing on 24-7-30.
//

#ifndef GROUPUSER_HPP
#define GROUPUSER_HPP

#include <bits/stdc++.h>
#include "user.hpp"

using namespace std;

class GroupUser :public User{
public:
    void setRole(string role){this->role=role;}
    string getRole() {
        return this->role;
    }
private:
    string role;
};

#endif //GROUPUSER_HPP
