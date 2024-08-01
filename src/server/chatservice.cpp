//
// Created by turing on 24-7-29.
//
#include <chatservice.hpp>
#include <public.hpp>
#include <muduo/base/Logging.h>
#include "offlinemessagemodel.hpp"

ChatService *ChatService::instance() {
    static ChatService ins;
    return &ins;
}

ChatService::ChatService() {
    _msgHandlerMap[LOGIN_MSG] = std::bind(&ChatService::login, this, _1, _2, _3);
    _msgHandlerMap[REG_MSG] = std::bind(&ChatService::reg, this, _1, _2, _3);
    _msgHandlerMap[ONE_CHAT_MSG] = std::bind(&ChatService::oneChat, this, _1, _2, _3);
    _msgHandlerMap[ADD_FRIEND_MSG] = std::bind(&ChatService::addFriend, this, _1, _2, _3);
    _msgHandlerMap[CREATE_GROUP_MSG] = std::bind(&ChatService::createGroup, this, _1, _2, _3);
    _msgHandlerMap[ADD_GROUP_MSG] = std::bind(&ChatService::addGroup, this, _1, _2, _3);
    _msgHandlerMap[GROUP_CHAT_MSG] = std::bind(&ChatService::groupChat, this, _1, _2, _3);
    _msgHandlerMap[LOGINOUT_MSG] = std::bind(&ChatService::loginout, this, _1, _2, _3);

    if (_redis.connect()) {
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int id = js["id"];
    string pwd = js["password"];
    User user = _userModel.query(id);
    if (user.getId() == id && user.getPwd() == pwd) {
        json response;
        if (user.getState() == "online") {
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using";
            conn->send(response.dump());
        } else {
            //登录成功,记录用户链接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap[id] = conn;
            }
            //向redis订阅channel(id)
            _redis.subscribe(id);
            //更新用户状态信息
            user.setState("online");
            _userModel.updateState(user);

            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            //查询该用户是否有离线消息
            auto vec = _offlineMsgModel.query(id);
            if (!vec.empty()) {
                response["offlinemsg"] = vec;
                _offlineMsgModel.remove(id);
            }
            //查询用户的好友信息并返回
            auto user_vec = _friendModel.query(id);
            if (!user_vec.empty()) {
                vector<string> vec2;
                for (auto usr: user_vec) {
                    json jstmp;
                    jstmp["id"] = usr.getId();
                    jstmp["name"] = usr.getName();
                    jstmp["state"] = usr.getState();
                    vec2.push_back(jstmp.dump());
                }
                response["friends"] = vec2;
            }

            // 查询用户的群组信息
            vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty()) {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group: groupuserVec) {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user: group.getUsers()) {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }

                response["groups"] = groupV;
            }

            conn->send(response.dump());
        }
    } else {
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "username or passwoed error";
        conn->send(response.dump());
    }
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    string name = js["name"];
    string pwd = js["password"];
    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state) {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    } else {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
}


MsgHandler ChatService::getHandler(int msgid) {
    //记录错误日志,msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()) {
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp) {
            LOG_ERROR << "msgid" << msgid << "can not find handler!";
        };
    } else return _msgHandlerMap[msgid];
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn) {
    User user; {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++) {
            if (it->second == conn) {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    _redis.unsubscribe(user.getId());

    if (user.getId() != -1) {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int toid = js["toid"];
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end()) {
            //在线,转发消息
            it->second->send(js.dump());
            return;
        }
    }
    //查询toid是否在别的机器上在线
    User user=_userModel.query(toid);
    if(user.getState()=="online") {
        _redis.publish(toid,js.dump());
        return;
    }
    //不在线,存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

void ChatService::reset() {
    _userModel.resetState();
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"];
    int friendid = js["friendid"];

    _friendModel.insert(userid, friendid);
}

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"];
    string name = js["groupname"];
    string desc = js["groupdesc"];

    Group group(-1, name, desc);
    if (_groupModel.createGroup(group)) {
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"];
    int groupid = js["groupid"];
    _groupModel.addGroup(userid, groupid, "normal");
}

void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"];
    int groupid = js["groupid"];
    vector<int> gsr_vec = _groupModel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex);
    for (auto id: gsr_vec) {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end()) {
            //用户在当前服务器上且在线
            it->second->send(js.dump());
        } else {
            //用户在别的服务器,查看是否在线
            User user=_userModel.query(id);
            if(user.getState()=="online") {
                _redis.publish(id,js.dump());
            }
            else _offlineMsgModel.insert(id, js.dump());
        }
    }
}

void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userid = js["id"]; {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end()) {
            _userConnMap.erase(it);
        }
    }
    //用户注销,下线,在redis中取消订阅
    _redis.unsubscribe(userid);
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}
void ChatService::handleRedisSubscribeMessage(int userid, string msg) {
    lock_guard<mutex>lock(_connMutex);
    auto it=_userConnMap.find(userid);
    if(it!=_userConnMap.end()) {
        it->second->send(msg);
        return;
    }
    _offlineMsgModel.insert(userid,msg);
}
