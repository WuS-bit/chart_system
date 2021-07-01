#ifndef LOGICSERVER_H
#define LOGICSERVER_H

#include "protocol.h"

// 提供对客户端请求的各种接口,都作为业务线程回调函数使用
class LogicServer
{
private:
    /* data */
public:
    LogicServer(/* args */);
    ~LogicServer();
    // 业务接口函数
    ALLOC_ACCOUNT * do_alloc_account();
    RESPONSE_STATUS * do_register(char *account, char *username, char *password);
    LOGIN_RESPONSE * do_login(char *account, char *password);
    RESPONSE_FRIEND_LIST * do_get_friend_list(char *account);
    CHART_ONE * do_chart_one(CHART_ONE * content);
    SERV_CHART_RECORD * do_get_chart_record(char * sender, char *recver, unsigned long long num);
};

LogicServer::LogicServer(/* args */)
{
}

LogicServer::~LogicServer()
{
}

LogicServer logicServer;

#endif