#include "include/TcpConnection.h"

TcpConnection::TcpConnection(int fd, sockaddr_in *addr)
{
    this->sockfd = fd;
    this->clnt_addr = new InetAddress(addr);
}

TcpConnection::~TcpConnection()
{
    delete clnt_addr;
}

void TcpConnection::setEpfd(int epfd)
{
    this->epfd = epfd;
}

char *TcpConnection::getPeerInfo()
{
    return clnt_addr->getIpPort();
}

int TcpConnection::getSockfd()
{
    return this->sockfd;
}

InetAddress* TcpConnection::getInetAddr()
{
    return this->clnt_addr;
}
