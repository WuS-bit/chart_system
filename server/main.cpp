#include "include/net/TcpServer.h"

int main(int argc, char *argv[])
{
    TcpServer *server = new TcpServer(argv[1]);
    server->run();

    return 0;
}