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

void TcpConnection::setEventLoop(EventLoop *loop)
{
    this->eventLoop = loop;
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
            pool->produceTask(task);

            // 取出执行结果，发起响应IO


            // 本次请求彻底响应完毕，销毁任务

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
            pool->produceTask(task);
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
            pool->produceTask(task);
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
            pool->produceTask(task);
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
            pool->produceTask(task);
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
            pool->produceTask(task);
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

// 只负责网络通信功能，不面向协议
void TcpConnection::onWriteMsg(char *buf, size_t len)
{
    // 执行网络发送功能，利用应用层缓冲区

    // 如果应用层缓冲区有数据，先写入缓冲区
    if (sendBuffer.getRemaining() > 0)
    {
        sendBuffer.writeBuffer(len, buf);

        // 添加可写触发事件
        this->eventLoop->modfd(this->sockfd, EPOLLOUT);
    }
    else
    {
        // 直接利用套接字发送，若内和缓冲区已满，则先放入应用缓冲区
        long ret = send(sockfd, buf, len, 0);
        if (ret >= 0 && ret == len)
        {
            // 全部写入内核缓冲区
            return;
        }
        else if (ret < 0)
        {
            // 出错
        }
        else
        {
            // 没写完，加入应用层缓冲区
            sendBuffer.writeBuffer(len-ret, buf+ret);
            this->eventLoop->modfd(sockfd, EPOLLOUT);
        }     
    }
    
}

// 由EventLoop可写事件触发
void TcpConnection::onSend()
{
    size_t len = sendBuffer.getRemaining();
    char *buf = sendBuffer.readAllBuffer();

    // 调用实际发送函数
    onWriteMsg(buf, len);
}

EventLoop * TcpConnection::getEventLoop()
{
    return this->eventLoop;
}