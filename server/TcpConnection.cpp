#include "include/net/TcpConnection.h"

TcpConnection::TcpConnection(int fd, sockaddr_in *addr)
{
    this->sockfd = fd;
    this->clnt_addr = new InetAddress(addr);
}

TcpConnection::~TcpConnection()
{
    delete clnt_addr;
}

void TcpConnection::setEpfd(int epfd)
{
    this->epfd = epfd;
}

char *TcpConnection::getPeerInfo()
{
    return clnt_addr->getIpPort();
}

int TcpConnection::getSockfd()
{
    return this->sockfd;
}

InetAddress* TcpConnection::getInetAddr()
{
    return this->clnt_addr;
}

void TcpConnection::onMessageRecv(char *buf, size_t len)
{
    // 将数据添加到连接缓冲区
    this->recvBuffer.writeBuffer(len, buf);

    // 解析是否已经缓存了完整报文数据，若完整，则交付给业务逻辑处理，否则，继续接受数据
    if (recvBuffer.tryRead())
    {
        // 读取协议头以及数据，交付给业务逻辑处理,封装相应任务体交由业务处理线程池执行任务
        Header *header = recvBuffer.readHeader();
        // 判断将要调用业务逻辑的具体接口，封装任务交给线程池执行
        switch (header->type)
        {
        case USER_GET_ACCOUNT:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, USER_GET_ACCOUNT);
            // 调用分配账号接口

            // 绑定业务逻辑的回调函数
            std::function<ALLOC_ACCOUNT *()> callback;
            callback = std::bind(&LogicServer::do_alloc_account, logicServer);
            void * fptr = (void *)&callback;

            ThreadTask *task = new ThreadTask(this, fptr, USER_GET_ACCOUNT, data);        

            // 交付线程池执行即可


        }
        break;
        case USER_REGISTER:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, USER_REGISTER);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            std::function<RESPONSE_STATUS *(char *, char *, char *)> callback;
            callback = std::bind(&LogicServer::do_register, logicServer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            void * fptr = (void *)&callback;

            ThreadTask *task = new ThreadTask(this, fptr, USER_REGISTER, data);        

            // 交付线程池执行即可
        }
        break;
        case USER_LOGIN:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, USER_LOGIN);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            std::function<LOGIN_RESPONSE *(char *, char *)> callback;
            callback = std::bind(&LogicServer::do_login, logicServer, std::placeholders::_1, std::placeholders::_2);
            void * fptr = (void *)&callback;

            ThreadTask *task = new ThreadTask(this, fptr, USER_LOGIN, data);        

            // 交付线程池执行即可
        }
        break;
        case USER_GET_FRIEND_LIST:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, USER_GET_FRIEND_LIST);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            std::function<RESPONSE_FRIEND_LIST *(char *)> callback;
            callback = std::bind(&LogicServer::do_get_friend_list, logicServer, std::placeholders::_1);
            void * fptr = (void *)&callback;

            ThreadTask *task = new ThreadTask(this, fptr, USER_GET_FRIEND_LIST, data);        

            // 交付线程池执行即可
        }
        break;
        case CHART_ONE_INTERFACE:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, CHART_ONE_INTERFACE);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            std::function<CHART_ONE *(CHART_ONE *)> callback;
            callback = std::bind(&LogicServer::do_chart_one, logicServer, std::placeholders::_1);
            void * fptr = (void *)&callback;

            ThreadTask *task = new ThreadTask(this, fptr, CHART_ONE_INTERFACE, data);        

            // 交付线程池执行即可
        }
        break;
        case USER_GET_CHART_RECORD:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, USER_GET_CHART_RECORD);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            std::function<SERV_CHART_RECORD *(char *, char *, unsigned long long)> callback;
            callback = std::bind(&LogicServer::do_get_chart_record, logicServer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            void * fptr = (void *)&callback;

            ThreadTask *task = new ThreadTask(this, fptr, USER_GET_CHART_RECORD, data);        

            // 交付线程池执行即可
        }
        break;
        default:
            break;
        }
    }
    else
    {
        // 继续接受数据，直到形成完整报文
        return;
    }
    
}

void TcpConnection::onWriteMsg(char *buf, size_t len)
{

}

void TcpConnection::onSend()
{

}