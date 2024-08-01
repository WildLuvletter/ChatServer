//
// Created by turing on 24-6-8.
//

#include "Pool.h"

Pool *Pool::getPool() {
    static Pool pool;
    return &pool;
}

bool Pool::loadCOnfigFile() {
    FILE *pf = fopen("../config/mysql.conf", "r");

    if (pf == nullptr) {
        cout << "not exit" << endl;
        return false;
    }
    while (!feof(pf)) {
        char line[1024] = {0};
        fgets(line, 1024, pf);
        string str = line;
        int idx = str.find('=', 0);
        if (idx == -1) {
            continue;
        }
        int endidx = str.find('\n', idx);
        string key = str.substr(0, idx);
        string value = str.substr(idx + 1, endidx - idx - 1);
        if (key == "ip")
            _ip = value;
        else if (key == "port")
            _port = stoi(value);
        else if (key == "username")
            _username = value;
        else if (key == "password")
            _password = value;
        else if (key == "dbname")
            _dbname = value;
        else if (key == "initSize")
            _initSize = stoi(value);
        else if (key == "maxSize")
            _maxSize = stoi(value);
        else if (key == "maxIdleTime")
            _maxIdleTime = stoi(value);
        else if (key == "connectionTime")
            _connectionTime = stoi(value);
    }
    return true;
}

Pool::Pool() {
    if (!loadCOnfigFile()) {
        return;
    }
    for (int i = 0; i < _initSize; ++i) {
        Connect *p = new Connect;
        p->connect(_ip, _port, _username, _password, _dbname);
        p->refreshAliveTime();
        _connectiopnQue.push(p);
        _connectionCnt++;
    }
    //create producer thread
    thread produce(std::bind(&Pool::produceConnectionTask, this));
    produce.detach();
    //create a thread scanning dummy idle copnnection
    thread scanner(std::bind(&Pool::scannerConnectionTask, this));
    scanner.detach();
}


void Pool::scannerConnectionTask() {
    for (;;) {
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));
        unique_lock<mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize) {
            Connect *p = _connectiopnQue.front();
            if (p->getAliveTime() >= (_maxIdleTime * 1000)) {
                _connectiopnQue.pop();
                _connectionCnt--;
                delete p;
            }
            else {
                break;
            }
        }
    }
}

void Pool::produceConnectionTask() {
    for (;;) {
        unique_lock<mutex> lock(_queueMutex);
        while (!_connectiopnQue.empty()) {
            cv.wait(lock); //queue is not empty,waiting
        }

        if (_connectionCnt >= _maxSize) {
            Connect *p = new Connect;
            p->connect(_ip, _port, _username, _password, _dbname);
            p->refreshAliveTime();
            _connectiopnQue.push(p);
            _connectionCnt++;
        }
        // notice consumer thread
        cv.notify_all();
    }
}

shared_ptr<Connect> Pool::getConnect() {
    unique_lock<mutex> lock(_queueMutex);
    while (_connectiopnQue.empty()) {
        if (cv.wait_for(lock, chrono::microseconds(_connectionTime)) == cv_status::timeout) {
            if (_connectiopnQue.empty()) {
                cout << "time out" << endl;
                return nullptr;
            }
        }
    }
    // shared_ptr<Connect> sp(_connectiopnQue.front());
    shared_ptr<Connect> sp(_connectiopnQue.front(),
                           [&](Connect *pcon) {
                               unique_lock<mutex> lock(_queueMutex);
                               pcon->refreshAliveTime();
                               _connectiopnQue.push(pcon);
                           });
    _connectiopnQue.pop();
    if (_connectiopnQue.empty())cv.notify_all();
    return sp;
}
