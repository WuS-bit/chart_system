#include "include/chart_logic/LogicServer.h"

std::map<std::string, int> user;
std::map<int, std::string> find_user; // 文件描述符作键值

LogicServer::LogicServer()
{
    this->account = 100000000000;
}

LogicServer::~LogicServer()
{
}

LogicServer logicServer;


ALLOC_ACCOUNT * LogicServer::do_alloc_account()
{
    ALLOC_ACCOUNT *ptr = (ALLOC_ACCOUNT *)malloc(sizeof(ALLOC_ACCOUNT));
    // 分配账号
    
    sprintf(ptr->account, "%lld", this->account);
    strcpy(ptr->msg, "请求成功");
    ptr->code = 200;
    // 此处线程不安全
    this->account++;

    return ptr;
}

// 废弃接口
RESPONSE_STATUS * LogicServer::do_register(char *account, char *username, char *password)
{
    // 注册逻辑

}

LOGIN_RESPONSE * LogicServer::do_login(char *account, char *password)
{
    // 用户登录，返回个人信息，并将上线信息广播给在线用户
    LOGIN_RESPONSE *ptr = (LOGIN_RESPONSE *)malloc(sizeof(LOGIN_RESPONSE));
    ptr->code = 200;
    strcpy(ptr->msg, "登录成功!");
    strcpy(ptr->account, account);
    strcpy(ptr->username, account);
    strcpy(ptr->password, ptr->password);

    // 记录在线状态,外面记录
    return ptr;

}
// 获取在线好友接口
RESPONSE_FRIEND_LIST * LogicServer::do_get_friend_list(const char *account)
{   
    string my_name = account;

    RESPONSE_FRIEND_LIST *list = (RESPONSE_FRIEND_LIST *)malloc(sizeof(RESPONSE_FRIEND_LIST));

    list->friend_num = user.size()-1;
    list->array = new FriendInfo[list->friend_num];
    // 构造在线好友列表
    int i=0;
    map<int, string>::iterator ite;
    ite = find_user.begin();
    while (ite != find_user.end())
    {
        // 不是自己的用户
        if (ite->second != my_name)
        {
            string username = ite->second;  
            list->array[i++].setAccount(username.c_str());
        }
    }

    return list;

}
CHART_ONE * LogicServer::do_chart_one(CHART_ONE * content)
{
    //只需保存聊天记录
}
SERV_CHART_RECORD * LogicServer::do_get_chart_record(char * sender, char *recver, unsigned long long num)
{
    // 读取文件获取聊天记录
}

CHART_GROUP * LogicServer::do_chart_group(CHART_GROUP *)
{
    // 向群聊记录中写入
}

SERV_GROUP_RECORD * LogicServer::do_get_group_record()
{
    // 读取群聊文件
}
