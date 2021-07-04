#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <unistd.h>
#include <cstdio>

#include "dao/FriendInfo.h"
#include "dao/ChartOneRecord.h"
#include "dao/ChartGroupRecord.h"

typedef struct Header
{
    unsigned int type; 
    unsigned long long length;
} Header;

// #define SERVER_SUCCESS 0
// typedef struct SUCCESS
// {
//     unsigned long long code = 200;
//     char msg[16]{"请求成功!\n"};
// } SUCCESS;

#define SERVER_STATUS 1
typedef struct RESPONSE_STATUS
{
    unsigned long long code;
    char msg[16];
} RESPONSE_STATUS;

//请求类型
#define USER_GET_ACCOUNT 2

// 响应类型,对于每种响应类型，统一保有响应状态字段，以标志本次请求是否成功
#define SERVER_ALLOC_ACCOUNT 3
typedef struct ALLOC_ACCOUNT
{
    unsigned long long code; // 响应状态码
    char msg[16];   // 响应消息，出错使用
    char account[15];
} ALLOC_ACCOUNT;

#define USER_REGISTER 4
typedef struct REGISTER
{
    char account[16];
    char username[32];
    char password[32];
} REGISTER;

#define USER_LOGIN 5
typedef struct LOGIN
{
    char account[16];
    char password[32];
} LOGIN;

#define SERVER_LOGIN_RESPONSE 6
typedef struct LOGIN_RESPONSE
{
    unsigned long long code; // 响应状态码
    char msg[16];   // 响应消息，出错使用
    char account[16];
    char username[32];
    char password[32];
} LOGIN_RESPONSE;

#define USER_GET_FRIEND_LIST 7
typedef struct GET_FRIEND_LIST
{
    char account[16];
} GET_FRIEND_LIST;

#define SERVER_RESPONSE_FRIEND_LIST 8
typedef struct RESPONSE_FRIEND_LIST
{
    unsigned long long friend_num; // 好友个数
    FriendInfo * array; // 好友数组
}RESPONSE_FRIEND_LIST;

#define CHART_ONE_INTERFACE 9
// 客户端的私聊请求以及服务器的主动推送
typedef struct CHART_ONE
{
    char sender[16];
    char recver[16];
    char msg[1024];
} CHART_ONE;

#define USER_GET_CHART_RECORD 10
typedef struct GET_CHART_RECORD
{
    char sender[16];
    char recver[16];
    unsigned long long num;
} GET_CHART_RECORD;

#define SERVER_CHART_RECORD 11
typedef struct SERV_CHART_RECORD
{
    unsigned long long num;
    ChartOneRecord *records;
}SERV_CHART_RECORD;

#define CHART_GROUP_INTERFACE 12
typedef struct CHART_GROUP
{
    char sender[16];
    char msg[1024];
}CHART_GROUP;

#define USER_GET_GROUP_RECORD 13 // 不需要请求数据
// typedef struct GET_GROUP_RECORD
// {
    
// }

#define SERVER_GROUP_RECORD 14
typedef struct SERV_GROUP_RECORD
{
    unsigned long long num;
    ChartGroupRecord *records;
} SERV_GROUP_RECORD;

#define USER_LOGOUT 15
typedef struct LOGOUT
{
    int sockfd;
}LOGOUT;



// struct BinaryBody
// {
    
// };



#endif