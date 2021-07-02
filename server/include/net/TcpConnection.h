#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <functional>

#include "Buffer.h"
#include "InetAddress.h"
#include "../chart_logic/ThreadTask.h"
#include "../chart_logic/Threadpool.h"
#include "LogicServer.h"

using namespace std;

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

    void onMessageRecv(char *buf, size_t len);

    void onSend();
    void onWriteMsg(char *buf, size_t len);
};

extern LogicServer logicServer; // 全局对象，仅用来提供回调函数
extern Threadpool *pool; // 全局业务线程池对象，每个连接均使用此线程池

#endif
