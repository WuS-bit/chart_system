#include "include/chart_logic/ThreadTask.h"

ThreadTask::ThreadTask(TcpConnection *conn, void *callback, int service, void *args)
{
    this->conn = conn;
    this->callbackFunc = callback;
    this->serviceType = service;
    this->args = args;
}

ThreadTask::~ThreadTask()
{
    delete conn;
    delete callbackFunc;
    delete args;
}