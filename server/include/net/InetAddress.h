#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>

typedef struct sockaddr_in sockaddr_in;

class InetAddress
{
private:
    sockaddr_in* addr;
public:
    InetAddress(sockaddr_in *addr);
    char* getIpPort();
    sockaddr_in* getSockAddr();
    ~InetAddress();
};

#endif