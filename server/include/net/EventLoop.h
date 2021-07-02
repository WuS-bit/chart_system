#ifndef EVENTLOOP_H
#define EVENTLOOP_H

// 封装epoll对象，从Reactor，对连接读写事件循环进行管理处置

#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <deque>
#include <cstdlib>

#ifndef CAS
#define CAS __sync_bool_compare_and_swap
#endif

#include "TcpConnection.h"

#define MAX_EVENT_NUM 1024

using namespace std;

// 引用其他文件定义的类，需要提前声明
class TcpConnection;
class Threadpool;

extern Threadpool *pool;
extern map<int, TcpConnection *> clnt_conns;

class EventLoop
{
private:
    int epfd; // epoll对象
    int efd;    // 事件通知机制，描述符
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
    int getEfd();

    // 相当于消息队列排队作用,避免响应数据乱序
    deque<std::function<void()>> io_cb_queue;

};

#endif