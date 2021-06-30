#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>
#include <netdb.h>

#define MAX_QUEUE_LENGTH 1024

class TcpServer
{
private:
    char *port;
public:
    TcpServer(char *port);
    ~TcpServer();
    int open_listenfd();
    void run();
};

#endif