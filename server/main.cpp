#include "include/net/TcpServer.h"
#include <dirent.h>

char *curDir;

int main(int argc, char *argv[])
{
    curDir = getcwd(NULL, 0);
    TcpServer *server = new TcpServer(argv[1]);
    server->run();

    return 0;
}