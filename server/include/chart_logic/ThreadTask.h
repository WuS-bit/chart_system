#ifndef THREADTASK_H
#define THREADTASK_H

#include "../net/TcpConnection.h"

class TcpConnection;

// 业务逻辑线程池通用任务，IO操作统一由从Reactor执行，线程池不执行
class ThreadTask
{
private:
    TcpConnection *conn;
    void * callbackFunc; // 实际是一个std函数包装对象, c++规定void *只能是非常量指针，函数指针是常量指针
    int serviceType;
    void * args;
    void * returnValue; // 一个入参，用于接受返回值数据
public:
    ThreadTask(TcpConnection *conn, void *callback, int service, void *args);
    ~ThreadTask();
    TcpConnection *getConn();
    void *getCallback();
    int getServiceType();
    void *getArgs();
};

#endif