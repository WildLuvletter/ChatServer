//
// Created by turing on 24-7-26.
//
#include <iostream>
#include <chatserver.hpp>
#include <bits/stdc++.h>
#include <json.hpp>
#include <chatservice.hpp>
#include <db/db.h>
#include <usermodel.hpp>
#include <user.hpp>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg): _server(loop, listenAddr, nameArg) {
    //设置链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    //设置消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    //设置线程数量
    _server.setThreadNum(4);
}

void ChatServer::start() {
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn) {
    //客户端断开链接
    if (!conn->connected()) {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buff, Timestamp time) {
    string buf = buff->retrieveAllAsString();
    json js = json::parse(buf);
    auto msghandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    //根据消息id调用对应的回调函数
    msghandler(conn, js, time);
}
