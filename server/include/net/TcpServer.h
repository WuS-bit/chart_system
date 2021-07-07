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
#include <map>

#include "EventLoopThreadpool.h"
#include "TcpConnection.h"

#define MAX_QUEUE_LENGTH 1024

using namespace std;

class TcpConnection;

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

extern map<int, TcpConnection *> *clnt_conns;

#endif