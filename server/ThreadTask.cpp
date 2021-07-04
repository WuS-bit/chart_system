#include "include/chart_logic/ThreadTask.h"

ThreadTask::ThreadTask(TcpConnection *conn, void *callback, int service, void *args, Threadpool *pool)
{
    this->conn = conn;
    this->callbackFunc = callback;
    this->serviceType = service;
    this->args = args;
    this->pool = pool;
}

ThreadTask::~ThreadTask()
{
    // delete conn;
    // delete callbackFunc;
    // delete args;
    // delete pool;    // 
}

TcpConnection *ThreadTask::getConn()
{
    return this->conn;
}
void *ThreadTask::getCallback()
{
    return this->callbackFunc;
}
int ThreadTask::getServiceType()
{
    return this->serviceType;
}
void *ThreadTask::getArgs()
{
    return this->args;
}

Threadpool * ThreadTask::getPool()
{
    return this->pool;
}