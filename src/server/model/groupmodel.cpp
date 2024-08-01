//
// Created by turing on 24-7-30.
//
#include "groupmodel.hpp"
#include "group.hpp"
#include "db.h"
#include "Connect.h"

void GroupModel::addGroup(int userid, int groupid, string role) {
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d,%d,'%s')", groupid, userid, role.c_str());
    // MySQL mysql;
    shared_ptr<Connect> mysql = Pool::getPool()->getConnect();
    mysql->update(sql);
}

vector<Group> GroupModel::queryGroups(int userid) {
    char sql[1024] = {0};
    sprintf(
        sql, "select a.id,a.groupname,a.groupdesc from allgroup a join "
        "groupuser b on a.id=b.groupid where b.userid=%d", userid);
    vector<Group> vec;
    // MySQL mysql;
    shared_ptr<Connect> mysql = Pool::getPool()->getConnect();
    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            Group group;
            group.setId(stoi(row[0]));
            group.setName(row[1]);
            group.setDesc(row[2]);
            vec.push_back(group);
        }
        mysql_free_result(res);
    }

    for (auto &group: vec) {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from user a "
                "join groupuser b on a.id=b.userid where b.groupid=%d", group.getId());

        MYSQL_RES *res = mysql->query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                GroupUser gsr;;
                gsr.setId(stoi(row[0]));
                gsr.setName(row[1]);
                gsr.setState(row[2]);
                gsr.setRole(row[3]);
                group.getUsers().push_back(gsr);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}

bool GroupModel::createGroup(Group &group) {
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname,groupdesc) values('%s', '%s')",
            group.getName().c_str(),
            group.getDesc().c_str());
    // MySQL mysql;
    shared_ptr<Connect> mysql = Pool::getPool()->getConnect();
    if (mysql->update(sql)) {
        group.setId(mysql_insert_id(mysql->getConnection()));
        return true;
    }

    return false;
}

vector<int> GroupModel::queryGroupUsers(int userid, int groupid) {
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid=%d and userid!=%d", groupid, userid);
    // MySQL mysql;
    shared_ptr<Connect> mysql = Pool::getPool()->getConnect();
    vector<int> vec;

    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            vec.push_back(stoi(row[0]));
        }
        mysql_free_result(res);
    }

    return vec;
}
