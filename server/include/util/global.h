#ifndef GLOBAL_H
#define GLOBAL_H

#include "../chart_logic/Threadpool.h"
#include "../net/TcpConnection.h"

map<int, TcpConnection *> clnt_conns;
Threadpool * pool = new Threadpool(CPU_NUM, MAXQUEUESIZE);

extern Threadpool *pool;
extern map<int, TcpConnection *> clnt_conns;

#endif