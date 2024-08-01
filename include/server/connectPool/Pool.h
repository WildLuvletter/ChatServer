//
// Created by turing on 24-6-8.
//

#ifndef POOL_H
#define POOL_H
#include<bits/stdc++.h>
#include "Connect.h"
using namespace std;

class Pool {
public:
    static Pool* getPool();
    shared_ptr<Connect> getConnect();
private:
    Pool();
    bool loadCOnfigFile();

    void produceConnectionTask();

    void scannerConnectionTask();
    string _ip;
    unsigned short _port;
    string _username;
    string _password;
    string _dbname;
    int _initSize;   // initial connect count
    int _maxSize;    // max connect count
    int _maxIdleTime;
    int _connectionTime;  //timeout time
    atomic_int _connectionCnt;  //total count of connection
    queue<Connect*>_connectiopnQue;
    mutex _queueMutex;
    condition_variable cv;   //comm in producer and consumer
};



#endif //POOL_H
