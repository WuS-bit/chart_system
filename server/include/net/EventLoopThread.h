#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include <pthread.h>

#include "EventLoop.h"
#include "TcpConnection.h"

class EventLoopThread
{
private:
    EventLoop *loop;
    pthread_t *thread;
public:
    EventLoopThread();
    ~EventLoopThread();
    void start();
    void run();
    bool addConn(TcpConnection *conn);
    bool modConnEvent();
    bool removeConn();
};


#endif