//
// Created by turing on 24-7-29.
//

#ifndef CHATSERVICE_HPP
#define CHATSERVICE_HPP
#include <groupmodel.hpp>
#include <muduo/net/TcpConnection.h>
#include <bits/stdc++.h>
#include <json.hpp>
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "redis.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

//处理处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

//聊天服务器业务类
class ChatService {
public:
    static ChatService *instance();

    //处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);

    //客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    //一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //服务器异常,业务充值方法
    void reset();

    //创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //从redis消息队列中获取订阅消息
    void handleRedisSubscribeMessage(int,string);


private:
    ChatService();

    //存储消息id和其对应的处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;
    //数据库操作类
    UserModel _userModel;
    //存储在线用户的通信链接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    mutex _connMutex;

    OfflineMsgModel _offlineMsgModel;

    FriendModel _friendModel;

    GroupModel _groupModel;

    Redis _redis;
};



#endif //CHATSERVICE_HPP
