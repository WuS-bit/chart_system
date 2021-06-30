#include "include/InetAddress.h"

InetAddress::InetAddress(sockaddr_in* address)
{
    addr = address;
}

sockaddr_in * InetAddress::getSockAddr()
{
    return addr;
}

char* InetAddress::getIpPort()
{
    char *buf = (char *)malloc(sizeof(char)*96);
    char host[64], port[32];
    getnameinfo((sockaddr *)addr, sizeof(sockaddr_in), host, 64, port, 32, NI_NUMERICHOST|NI_NUMERICSERV);
    strcpy(buf, host);
    size_t ipLen = strlen(buf);
    buf[ipLen]=':';
    strcpy(buf+ipLen+1, port);
    return buf;
}

InetAddress::~InetAddress()
{
    free(addr);
}
