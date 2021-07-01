#include "include/net/TcpServer.h"

TcpServer::TcpServer(char *port)
{
    this->port = port;
}

TcpServer::~TcpServer()
{
    delete this->port;
}

int TcpServer::open_listenfd()
{
    int listenfd;
    // 创建一个TCP监听套接字
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // 获取本机可用的套接字地址（IP+PORT）
    struct addrinfo *p, *listp, hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // 地址协议族
    hints.ai_socktype = SOCK_STREAM; // TCP套接字类型
    hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

    // 获取本机可用的指定类型的socket地址信息
    getaddrinfo(NULL, this->port, &hints, &listp);

    for (p=listp; p; p=p->ai_next)
    {
        // 尝试绑定socket
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
        {
            // 绑定成功
            break;
        }
    }

    if (!p)
    {
        // 获取监听套接字失败
        close(listenfd);
        return -1;
    }
    
    // 绑定成功，开始监听
    if (listen(listenfd, MAX_QUEUE_LENGTH) == 0)
    {
        // 开始监听，MAX_QUEUE_LENGTH为最大半连接队列长度
        return listenfd;
    }
    
    // 监听失败
    close(listenfd);
    return -1;
}

// 设置非阻塞
void setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}

void TcpServer::run()
{
    // 获取到可用的监听套接字 文件描述符
    int listenfd = open_listenfd();
    if (listenfd == -1)
    {
        perror("Server Error: Can't establish a listenfd!\n");
        exit(-1);
    }

    // 使用epoll创建事件循环对象
    int epfd = epoll_create1(EPOLL_CLOEXEC);

    // 添加监听套接字事件
    struct epoll_event event;
    event.data.fd = listenfd;
    event.events = EPOLLIN;

    setnonblocking(listenfd);

    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
    
    struct epoll_event events[5];

    struct sockaddr_in *clnt_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    // 创建从reactor资源池
    EventLoopThreadpool *pool = new EventLoopThreadpool();
    // 启动从reactor
    pool->start();

    while (1)
    {
        int ready_num = epoll_wait(epfd, events, 5, -1);

        for (int i = 0; i < ready_num; i++)
        {
            if (events[i].data.fd == listenfd && (events[i].events & EPOLLIN))
            {
                // 有新的连接
                clnt_addr = (struct sockaddr_in *)malloc(len);
                int sockfd = accept(listenfd, (struct sockaddr *)clnt_addr, &len);

                TcpConnection *conn = new TcpConnection(sockfd, clnt_addr);
                pool->addConn(conn);
            }          
        }     
    } 
}