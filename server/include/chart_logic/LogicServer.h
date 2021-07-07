#ifndef LOGICSERVER_H
#define LOGICSERVER_H

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include "protocol.h"

using namespace std;

extern char *curDir;

// 提供对客户端请求的各种接口,都作为业务线程回调函数使用
class LogicServer
{
private:
    long long account;
public:
    LogicServer();
    ~LogicServer();
    // 业务接口函数
    ALLOC_ACCOUNT * do_alloc_account();
    RESPONSE_STATUS * do_register(char *account, char *username, char *password);
    LOGIN_RESPONSE * do_login(char *account, char *password);
    RESPONSE_FRIEND_LIST * do_get_friend_list(const char *account);
    CHART_ONE * do_chart_one(CHART_ONE * content);
    SERV_CHART_RECORD * do_get_chart_record(char * sender, char *recver, unsigned long long num);
    CHART_GROUP * do_chart_group(CHART_GROUP *);
    SERV_GROUP_RECORD * do_get_group_record();
};

extern std::map<std::string, int> user;
extern std::map<int, std::string> find_user;

#endif