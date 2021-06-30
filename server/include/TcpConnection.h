#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Buffer.h"
#include "InetAddress.h"

class TcpConnection
{
private:
    int epfd; // 绑定到的epoll对象描述符
    int sockfd; //连接套接字描述符
    InetAddress * clnt_addr;
    Buffer recvBuffer;
    Buffer sendBuffer;
public:
    TcpConnection(int fd, sockaddr_in *clnt_addr);
    ~TcpConnection();

    void setEpfd(int epfd);
    
    int getSockfd();
    InetAddress *getInetAddr();
    char *getPeerInfo();
};


#endif
