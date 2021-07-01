#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <pthread.h>

#include "TcpConnection.h"
#include "EventLoopThread.h"

#define THREAD_NUM 5

class EventLoopThreadpool
{
private:
    EventLoopThread *loopThread;
    int round_pos;
public:
    EventLoopThreadpool();
    ~EventLoopThreadpool();
    void start();
    bool addConn(TcpConnection *conn);
};


#endif