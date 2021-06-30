#include "include/EventLoop.h"

EventLoop::EventLoop()
{
    // 创建epoll内核对象
    this->epfd = epoll_create1(EPOLL_CLOEXEC);
    if (this->epfd)
    {
        // 创建失败
    }
    this->events = (struct epoll_event *)malloc(sizeof(struct epoll_event)*MAX_EVENT_NUM);
}

EventLoop::~EventLoop()
{
}

// epoll事件循环
void EventLoop::loop()
{
    while (1)
    {
        int ready_num = epoll_wait(this->epfd, this->events, MAX_EVENT_NUM, -1);
        for (int i = 0; i < ready_num; i++)
        {
            if (this->events[i].events & EPOLLIN)
            {
                // 处理连接读事件
            }
            else if (this->events[i].events & EPOLLOUT)
            {
                // 处理写事件
            }
            
            
        }
        
    }
    
}


bool EventLoop::addConn()
{
    
}
