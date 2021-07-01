#include "include/net/EventLoopThreadpool.h"

EventLoopThreadpool::EventLoopThreadpool()
{
    this->loopThread = new EventLoopThread[THREAD_NUM];
    this->round_pos = 0;
}

EventLoopThreadpool::~EventLoopThreadpool()
{
    delete loopThread;
}

void EventLoopThreadpool::start()
{
    // 启动所有从reactor线程
    for (int i = 0; i < THREAD_NUM; i++)
    {
        this->loopThread[i].start();
    }
    
}

bool EventLoopThreadpool::addConn(TcpConnection *conn)
{
    // 轮询算法将新连接负载均衡到从Reactor上
    EventLoopThread *cur_loop = &(this->loopThread[this->round_pos]);
    this->round_pos = (++(this->round_pos))%THREAD_NUM;

    cur_loop->addConn(conn);
}