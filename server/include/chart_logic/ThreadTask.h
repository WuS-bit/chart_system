#ifndef THREADTASK_H
#define THREADTASK_H

#include "../net/TcpConnection.h"


// 业务逻辑线程池通用任务
class ThreadTask
{
private:
    TcpConnection *conn;
    void * callbackFunc; // 实际是一个std函数包装对象, c++规定void *只能是非常量指针，函数指针是常量指针
    int serviceType;
    void * args;
public:
    ThreadTask(TcpConnection *conn, void *callback, int service, void *args);
    ~ThreadTask();
};

#endif