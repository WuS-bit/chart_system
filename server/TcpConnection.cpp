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
            // std::function<ALLOC_ACCOUNT *()> callback;
            // callback = std::bind(&LogicServer::do_alloc_account, logicServer);
            // void * fptr = (void *)new std::function<ALLOC_ACCOUNT *()>(callback);;

            // ThreadTask *task = new ThreadTask(this, fptr, USER_GET_ACCOUNT, data, pool); 

            // ALLOC_ACCOUNT *p = (*((std::function<ALLOC_ACCOUNT *()> *)task->getCallback()))();
            // printf("%s\n", p->account);


            // fprintf(stdout, "交付给线程池\n");       

            // 交付线程池执行即可
            // pool->produceTask(task);

            // 得到业务处理回调函数并调用
            // std::function<ALLOC_ACCOUNT *()> *cb = (std::function<ALLOC_ACCOUNT *()> *)task->getCallback();
            ALLOC_ACCOUNT *res = logicServer->do_alloc_account();
            Header * header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_ALLOC_ACCOUNT;
            header->length = sizeof(ALLOC_ACCOUNT);


            // 写回响应数据
            this->onWriteMsg((char *)header, sizeof(Header));
            this->onWriteMsg((char *)res, sizeof(ALLOC_ACCOUNT));

            // 封装回调函数 
            // auto cb1 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)header, sizeof(Header));
            // auto cb2 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)res, sizeof(ALLOC_ACCOUNT));

            // this->getEventLoop()->io_cb_queue.push_back(new std::function<void()>(cb1));
            // this->getEventLoop()->io_cb_queue.push_back(new std::function<void()>(cb2));


            // uint64_t one = 1;
            // write(this->getEventLoop()->getEfd(), &one, sizeof(one));
            // fprintf(stdout, "线程池执行完任务\n");

            // 线程任务内存泄漏

        }
        break;
        case USER_REGISTER:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, USER_REGISTER);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            // std::function<RESPONSE_STATUS *(char *, char *, char *)> callback;
            // callback = std::bind(&LogicServer::do_register, logicServer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            // void * fptr = (void *)&callback;

            // ThreadTask *task = new ThreadTask(this, fptr, USER_REGISTER, data, pool);        

            // 交付线程池执行即可
            // pool->produceTask(task);

            // 在此线程中执行业务处理
            REGISTER * request = (REGISTER *)data;
            RESPONSE_STATUS *res = logicServer->do_register(request->account, request->username, request->password);

            // 响应头
            Header *header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_STATUS;
            header->length = sizeof(RESPONSE_STATUS); // 只保存此次数据长度

            this->onWriteMsg((char *)header, sizeof(Header));
            this->onWriteMsg((char *)res, sizeof(RESPONSE_STATUS));
        }
        break;
        case USER_LOGIN:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, USER_LOGIN);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            // std::function<LOGIN_RESPONSE *(char *, char *)> callback;
            // callback = std::bind(&LogicServer::do_login, logicServer, std::placeholders::_1, std::placeholders::_2);
            // void * fptr = (void *)&callback;

            // ThreadTask *task = new ThreadTask(this, fptr, USER_LOGIN, data, pool);        

            // 交付线程池执行即可
            // pool->produceTask(task);

            LOGIN * req = (LOGIN *)data;
            LOGIN_RESPONSE *res = logicServer->do_login(req->account, req->password);

            // 记录登录状态
            user.insert(std::pair<std::string, int>(res->account, this->sockfd));
            find_user.insert(std::pair<int, std::string>(this->sockfd, res->account));

            // 广播所有在线用户
            map<int, TcpConnection *>::iterator iter;
            iter = clnt_conns->begin();
            while (iter != clnt_conns->end())
            {
                printf("lll\n");
                TcpConnection *ptr = iter->second;
                if (ptr != this)
                {
                    // 返回在线列表
                    Header *header = (Header *)malloc(sizeof(Header));
                    header->type = SERVER_RESPONSE_FRIEND_LIST;
                    header->length = sizeof(RESPONSE_FRIEND_LIST) + sizeof(FriendInfo)*(user.size()-1);

                    // 若该连接对应用户已经登录，则获取用户名
                    if (find_user.find(iter->first) != find_user.end())
                    {
                        string username = find_user.find(iter->first)->second;

                        RESPONSE_FRIEND_LIST *list = logicServer->do_get_friend_list(username.c_str());

                        for (int i = 0; i < list->friend_num; i++)
                        {
                            printf("%s\n", list->array[i].username);
                        }
                    
                    
                        ptr->onWriteMsg((char *)header, sizeof(Header));
                        ptr->onWriteMsg((char *)list, sizeof(RESPONSE_FRIEND_LIST));
                    
                        ptr->onWriteMsg((char *)list->array, sizeof(FriendInfo)*list->friend_num);   
                    }
                }
                else
                {
                    // 回送登陆成功响应
                    Header *header = (Header *)malloc(sizeof(Header));
                    header->type = SERVER_LOGIN_RESPONSE;
                    header->length = sizeof(LOGIN_RESPONSE);

                    this->onWriteMsg((char *)header, sizeof(Header));
                    this->onWriteMsg((char *)res, sizeof(LOGIN_RESPONSE));
                }
                iter++;
            }
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

            ThreadTask *task = new ThreadTask(this, fptr, USER_GET_FRIEND_LIST, data, pool);        

            // 交付线程池执行即可
            // pool->produceTask(task)

            GET_FRIEND_LIST * req = (GET_FRIEND_LIST *)data;

            RESPONSE_FRIEND_LIST *res = callback(req->account);

            Header *header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_RESPONSE_FRIEND_LIST;
            header->length = sizeof(RESPONSE_FRIEND_LIST)+sizeof(FriendInfo)*res->friend_num;

            

            this->onWriteMsg((char *)header, sizeof(Header));
            this->onWriteMsg((char *)res, sizeof(RESPONSE_FRIEND_LIST));
            this->onWriteMsg((char *)res->array, sizeof(FriendInfo)*res->friend_num);
            
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

            ThreadTask *task = new ThreadTask(this, fptr, CHART_ONE_INTERFACE, data, pool);        

            // 交付线程池执行即可
            // pool->produceTask(task);

            CHART_ONE * req = (CHART_ONE *)data;
            CHART_ONE *res = callback(req);

            Header *header = (Header *)malloc(sizeof(Header));
            header->type = CHART_ONE_INTERFACE;
            header->length = sizeof(CHART_ONE);

            // 也给自己写消息
            this->onWriteMsg((char *)header, sizeof(Header));
            this->onWriteMsg((char *)res, sizeof(CHART_ONE));

            // 实质上需要向用户会送消息发送成功ACK信息，简化后不需要，直接推送消息
            string name = res->recver;
            int sockfd = user.find(name)->second;
            TcpConnection *conn = clnt_conns->find(sockfd)->second;

            conn->onWriteMsg((char *)header, sizeof(Header));
            conn->onWriteMsg((char *)res, sizeof(CHART_ONE));

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

            ThreadTask *task = new ThreadTask(this, fptr, USER_GET_CHART_RECORD, data, pool);        

            // 交付线程池执行即可
            // pool->produceTask(task);

            GET_CHART_RECORD * req = (GET_CHART_RECORD *)data;
            SERV_CHART_RECORD *res = callback(req->sender, req->recver, req->num);

            Header *header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_CHART_RECORD;
            header->length = sizeof(SERV_CHART_RECORD)+sizeof(ChartOneRecord)*res->num;

            this->onWriteMsg((char *)header, sizeof(Header));
            this->onWriteMsg((char *)res, sizeof(SERV_CHART_RECORD));
            this->onWriteMsg((char *)res->records, sizeof(ChartOneRecord)*res->num);
            
        }
        break;
        case CHART_GROUP_INTERFACE:
        {
            // 群发消息，将消息写入记录文件后，广播给其他在线用户

            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, CHART_GROUP_INTERFACE);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            std::function<CHART_GROUP *(CHART_GROUP *)> callback;
            callback = std::bind(&LogicServer::do_chart_group, logicServer, std::placeholders::_1);
            void * fptr = (void *)&callback;

            ThreadTask *task = new ThreadTask(this, fptr, USER_GET_CHART_RECORD, data, pool);        

            // 交付线程池执行即可
            // pool->produceTask(task);

            CHART_GROUP * req = (CHART_GROUP *)data;
            CHART_GROUP *res = callback(req);

            Header *header = (Header *)malloc(sizeof(Header));
            header->type = CHART_GROUP_INTERFACE;
            header->length = sizeof(CHART_GROUP);

            // 直接广播给所有用户，包括自己
            map<string, int>::iterator iter;
            iter = user.begin();
            while (iter!=user.end())
            {
                int sockfd = iter->second;
                TcpConnection *conn = clnt_conns->find(sockfd)->second;

                conn->onWriteMsg((char *)header, sizeof(Header));
                conn->onWriteMsg((char *)res, sizeof(CHART_GROUP));

                iter++;
            }
            
        }
        break;
        case USER_GET_GROUP_RECORD:
        {
            // 读取对应的数据
            void * data = this->recvBuffer.readData(header->length, USER_GET_GROUP_RECORD);
            // 调用用户注册接口

            // 绑定业务逻辑的回调函数
            std::function<SERV_GROUP_RECORD *()> callback;
            callback = std::bind(&LogicServer::do_get_group_record, logicServer);
            void * fptr = (void *)&callback;

            ThreadTask *task = new ThreadTask(this, fptr, USER_GET_CHART_RECORD, data, pool);        

            // 交付线程池执行即可
            // pool->produceTask(task);

            // GET_CHART_RECORD * req = (GET_CHART_RECORD *)data;
            SERV_GROUP_RECORD *res = callback();

            Header *header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_GROUP_RECORD;
            header->length = sizeof(SERV_GROUP_RECORD)+sizeof(ChartGroupRecord)*res->num;

            this->onWriteMsg((char *)header, sizeof(Header));
            this->onWriteMsg((char *)res, sizeof(SERV_GROUP_RECORD));
            this->onWriteMsg((char *)res->records, sizeof(ChartGroupRecord)*res->num);
            
        }
        break;
        case USER_LOGOUT:
        {
            // 用户下线，更新服务器在线列表，并通知在线用户
            
            LOGOUT *req = (LOGOUT *)this->recvBuffer.readData(header->length, USER_LOGOUT);

            string name = find_user.find(req->sockfd)->second;
            find_user.erase(req->sockfd);
            user.erase(name);

            map<string, int>::iterator iter;
            iter = user.begin();
            while (iter != user.end())
            {
                string username = iter->first;
                int sockfd = iter->second;
                TcpConnection *conn = clnt_conns->find(sockfd)->second;

                RESPONSE_FRIEND_LIST *list = logicServer->do_get_friend_list(username.c_str());

                Header *header = (Header *)malloc(sizeof(Header));
                header->type = SERVER_RESPONSE_FRIEND_LIST;
                header->length = sizeof(RESPONSE_FRIEND_LIST)+sizeof(FriendInfo)*list->friend_num;

                conn->onWriteMsg((char *)header, sizeof(Header));
                conn->onWriteMsg((char *)list, sizeof(RESPONSE_FRIEND_LIST));
                conn->onWriteMsg((char *)list->array, sizeof(FriendInfo)*list->friend_num);

                iter++;
            }
            
        }
        break;
        case USER_TRANS_FILE:
        {
            TRANS_FILE *data = (TRANS_FILE *)recvBuffer.readData(sizeof(TRANS_FILE), USER_TRANS_FILE);
            if (strcmp(data->recv, "000000000000") == 0)
            {
                // 群发
                map<int, TcpConnection *>::iterator iter;
                iter = clnt_conns->begin();
                while (iter != clnt_conns->end())
                {
                    TcpConnection *conn = iter->second;

                    Header *header = (Header *)malloc(sizeof(Header));
                    header->type = USER_TRANS_FILE;
                    header->length = sizeof(TRANS_FILE) + data->fsize;

                    conn->onWriteMsg((char *)header, sizeof(Header));

                    conn->onWriteMsg((char *)data, sizeof(TRANS_FILE));


                    char *path = getcwd(NULL, 0);
                    char filepath[255];
                    int len = strlen(path);
                    if (path[len-1] != '/')
                    {
                        path[len] = '/';
                        path[len + 1] = '\0';
                    }
                    strcpy(filepath, path);
                    strcat(filepath, data->filename);

                    

                    FILE *file = fopen(filepath, "rb");

                    int ret=0, r=data->fsize;
                    char buf[1024];

                    if (file != NULL)
                    {
                        ret = fread(buf, 1, 1024, file);
                        conn->onWriteMsg(buf, ret);
                        
                        r -= ret;
                        while (r > 0)
                        {
                            ret = fread(buf, 1, 1024, file);
                            conn->onWriteMsg(buf, ret);
                            r -= ret;
                        }
                    }

                    iter++;
                }
                
            } 
            else 
            {
                // 私发
                string name = data->recv;
                int sockfd = user.find(name)->second;
                TcpConnection *conn = clnt_conns->find(sockfd)->second;
                Header *header = (Header *)malloc(sizeof(Header));
                header->type = USER_TRANS_FILE;
                header->length = sizeof(TRANS_FILE) + data->fsize;

                conn->onWriteMsg((char *)header, sizeof(Header));

                conn->onWriteMsg((char *)data, sizeof(TRANS_FILE));


                char *path = getcwd(NULL, 0);
                char filepath[255];
                int len = strlen(path);
                if (path[len-1] != '/')
                {
                    path[len] = '/';
                    path[len + 1] = '\0';
                }
                strcpy(filepath, path);
                strcat(filepath, data->filename);

                    

                FILE *file = fopen(filepath, "rb");

                int ret=0, r=data->fsize;
                char buf[1024];

                if (file != NULL)
                {
                    ret = fread(buf, 1, 1024, file);
                    conn->onWriteMsg(buf, ret);
                        
                    r -= ret;
                    while (r > 0)
                    {
                        ret = fread(buf, 1, 1024, file);
                        conn->onWriteMsg(buf, ret);
                        r -= ret;
                    }
                }
            }
            
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
            this->eventLoop->modfd(sockfd, EPOLLIN);
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