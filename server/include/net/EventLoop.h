#ifndef EVENTLOOP_H
#define EVENTLOOP_H

// 封装epoll对象，从Reactor，对连接读写事件循环进行管理处置

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <cstdlib>

#include "TcpConnection.h"

#define MAX_EVENT_NUM 1024

using namespace std;

extern map<int, TcpConnection *> clnt_conns;

class EventLoop
{
private:
    int epfd; // epoll对象
    struct epoll_event *events; // 就绪事件数组

public:
    EventLoop();
    ~EventLoop();
    void loop();
    void setnonblocking(int fd);
    void addfd(int fd);
    void delfd(int fd);
    void modfd(int fd, int interest);
    bool addConn(TcpConnection *);

};

#endif