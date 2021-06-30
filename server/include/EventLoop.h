#ifndef EVENTLOOP_H
#define EVENTLOOP_H

// 封装epoll对象，从Reactor，对连接读写事件循环进行管理处置

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <cstdlib>

#define MAX_EVENT_NUM 1024

class EventLoop
{
private:
    int epfd; // epoll对象
    struct epoll_event *events; // 就绪事件数组

public:
    EventLoop();
    ~EventLoop();
    void loop();
    bool addConn();


};

#endif