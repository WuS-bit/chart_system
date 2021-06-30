#include "TcpServer.h"

TcpServer::TcpServer(char *port)
{
    this->port = port;
}

TcpServer::~TcpServer()
{
    delete port;
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

void TcpServer::run()
{
    int listenfd = open_listenfd();
    
}