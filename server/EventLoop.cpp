#include "include/net/EventLoop.h"

EventLoop::EventLoop()
{
    // 创建epoll内核对象
    this->epfd = epoll_create1(EPOLL_CLOEXEC);
    if (this->epfd)
    {
        // 创建失败
    }
    this->events = (struct epoll_event *)malloc(sizeof(struct epoll_event)*MAX_EVENT_NUM);

    efd = eventfd(0, 0);
    addfd(efd);
}

EventLoop::~EventLoop()
{
    close(efd);
    close(epfd);
    delete events;
}

int EventLoop::getEfd()
{
    return efd;
}

void EventLoop::setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}

// 对epoll对象操作，将连接套接字的某种事件监听加入到epoll中
void EventLoop::addfd(int fd)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(struct epoll_event));
    event.data.fd = fd;
    event.events = EPOLLIN; // 初始只监听可读事件
    this->setnonblocking(fd);
    epoll_ctl(this->epfd, EPOLL_CTL_ADD, fd, &event);
}

// 删除某个套接字
void EventLoop::delfd(int fd)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
}

// 修改epoll对某个套接字的关注事件
void EventLoop::modfd(int fd, int interest)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(struct epoll_event));
    event.data.fd = fd;
    event.events = EPOLLIN | interest;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}


// epoll事件循环
void EventLoop::loop()
{
    while (1)
    {
        int ready_num = epoll_wait(this->epfd, this->events, MAX_EVENT_NUM, -1);
        for (int i = 0; i < ready_num; i++)
        {

            // 事件通知事件，处理积累的IO事件
            // 暂时废弃
            if (this->events[i].data.fd == efd)
            {
                printf("lail\n");
                uint64_t one = 1;
                read(efd, &one, sizeof(one));
                // 需要操作回调队列，使用循环CAS
                while (!CAS(&pool->flag, 0, 1))
                {
                }
                
                // 保证线程安全
                while (io_cb_queue.size() > 0)
                {
                    auto cb = io_cb_queue.front();
                    io_cb_queue.pop_front();

                    // 执行回调函数，进行IO（写socket）
                    (*cb)();
                }
                CAS(&pool->flag, 1, 0);
                continue;
            }
            

            // 获取到连接套接字
            int sockfd = events[i].data.fd;
            TcpConnection *conn = NULL;
            map<int, TcpConnection *>::iterator ite;
            ite = clnt_conns->find(sockfd);
            if (ite != clnt_conns->end())
            {
                conn = ite->second;
            }
            
            // 数据缓冲区
            char *buf = (char *)malloc(1024);

            if (this->events[i].events & EPOLLIN)
            {
                // 处理连接读事件
                while (1)
                {
                    memset(buf, 0, 1024);

                    int ret = recv(sockfd, buf, 1024-1, 0);

                    if (ret < 0)
                    {
                        // 非阻塞ET模式下，下面条件成立时，表示已经读空了内核空间的TCP Socket缓冲区
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                        {
                            // 该条件下，可以退出此次读取
                            break;
                        }
                        // 异常情况，表示连接发生异常，断开连接
                        delete conn;
                        clnt_conns->erase(sockfd);
                        delfd(sockfd);
                        close(sockfd);
                        break;
                    }
                    else if (ret == 0)
                    {
                        // 表示对方连接已关闭，对方下线也要做些逻辑处理
                        Header *header = (Header *)malloc(sizeof(Header));
                        header->type = USER_LOGOUT;
                        header->length = sizeof(LOGOUT);

                        LOGOUT *req = (LOGOUT *)malloc(sizeof(LOGOUT));
                        req->sockfd = this->events[i].data.fd;

                        char b[sizeof(Header)+sizeof(LOGOUT)+1];
                        memcpy(b, header, sizeof(Header));
                        memcpy(b+sizeof(Header), req, sizeof(LOGOUT));
                        
                        // 这里会改变用户在线状态
                        conn->onMessageRecv(b, sizeof(Header)+sizeof(LOGOUT));

                        printf("close conn\n");


                        delete conn;
                        clnt_conns->erase(sockfd);
                        delfd(sockfd);
                        close(sockfd);
                        break;
                    }
                    else
                    {
                        // 正常读取数据，添加数据到连接的缓冲区，直至获得一次完整的协议报文
                        conn->onMessageRecv(buf, ret);
                    }
                }
            }
            else if (this->events[i].events & EPOLLOUT)
            {
                // 处理写事件
                conn->onSend();
            }
                
        }
        
    }
    
}


bool EventLoop::addConn(TcpConnection *conn)
{
    // 绑定连接对应的EventLoop
    conn->setEventLoop(this);

    int sockfd = conn->getSockfd();
    // 将连接对象套接字加到epoll对象上管理
    this->addfd(sockfd);
    // 将连接加到全局连接管理数据结构中
    clnt_conns->insert(pair<int, TcpConnection*>(sockfd, conn));
    // printf("%d\n",clnt_conns->size());

    return true;
}
