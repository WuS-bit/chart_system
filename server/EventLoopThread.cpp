#include "include/net/EventLoopThread.h"

static void * start_routine(void *arg)
{
    EventLoopThread *loop = (EventLoopThread *)arg;
    loop->run();
}

EventLoopThread::EventLoopThread()
{
    this->thread = (pthread_t *)malloc(sizeof(pthread_t));
    this->loop = new EventLoop();
}

EventLoopThread::~EventLoopThread()
{
    delete thread;
    delete loop;
}

void EventLoopThread::start()
{
    // 启动新线程
    pthread_create(this->thread, NULL, start_routine, (void *)this);
}

bool EventLoopThread::addConn(TcpConnection *conn)
{
    this->loop->addConn(conn);
}

void EventLoopThread::run()
{
    this->loop->loop();
}