#include "include/chart_logic/Threadpool.h"

Threadpool::Threadpool(int threadNums, int maxQueueSize)
{
    this->flag = 0;
    this->locker = new MutexLock();
    this->notEmpty = new Condition(this->locker);
    this->notFull = new Condition(this->locker);
    this->coreThreadNum = threadNums;
    this->maxQueueSize = maxQueueSize;
    this->curAliveThreadNUm = 0;
    this->coreThreads = (pthread_t *)malloc(this->coreThreadNum*(sizeof(pthread_t)));
}

Threadpool::~Threadpool()
{
    delete locker;
    delete notEmpty;
    delete notFull;
}

// 线程执行例程
static void * worker(void * args)
{
    ThreadArgs *data = (ThreadArgs *)args;
    ThreadTask *task = data->task;
    Threadpool *pool = data->pool;

    // 不停执行任务队列中的任务
    while (1)
    {
        pool->excuteTask(task);

        // 更改为由从Reactor执行IO，因此，线程池中不能销毁任务
        // delete task;
        task = pool->consumeTask();
    }
    
}

void Threadpool::excuteTask(ThreadTask *task)
{
    // 分析任务体，执行对应业务逻辑函数
    int serviceType = task->getServiceType();
    TcpConnection *conn = task->getConn();

    switch (serviceType)
    {
        case USER_GET_ACCOUNT:
        {
            // 得到业务处理回调函数并调用
            std::function<ALLOC_ACCOUNT *()> *cb = (std::function<ALLOC_ACCOUNT *()> *)task->getCallback();
            ALLOC_ACCOUNT *data = (*cb)();

            Header * header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_ALLOC_ACCOUNT;
            header->length = sizeof(ALLOC_ACCOUNT);

            // 封装回调函数 
            auto cb1 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)header, sizeof(Header));
            auto cb2 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)data, sizeof(ALLOC_ACCOUNT));

            // 循环CAS
            while(!CAS(&this->flag, 0, 1)) ;

            // 线程安全下添加
            conn->getEventLoop()->io_cb_queue.push_back(cb1);
            conn->getEventLoop()->io_cb_queue.push_back(cb2);

            CAS(&this->flag, 1, 0);

        }
        break;
        case USER_REGISTER:
        {
            // 获取业务处理回调函数
            std::function<RESPONSE_STATUS *(char *, char *, char *)> *cb = (std::function<RESPONSE_STATUS *(char *, char *, char *)> *)task->getCallback();
            // 获取业务请求数据
            REGISTER * data = (REGISTER *)task->getArgs();
            // 调用业务逻辑函数
            RESPONSE_STATUS *response = (*cb)(data->account, data->username, data->password);

            // 封装响应数据到IO线程
            Header *header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_STATUS;
            header->length = sizeof(RESPONSE_STATUS);

            // 回调函数
            std::function<void()> cb1 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)header, sizeof(Header));
            std::function<void()> cb2 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)response, sizeof(RESPONSE_STATUS));

            while (!CAS(&this->flag, 0, 1)) ;

            // 线程安全下添加
            conn->getEventLoop()->io_cb_queue.push_back(cb1);
            conn->getEventLoop()->io_cb_queue.push_back(cb2);

            CAS(&this->flag, 1, 0);

        }
        break;
        case USER_LOGIN:
        {
            // 获取业务处理回调函数
            std::function<LOGIN_RESPONSE *(char *, char *)> *cb = (std::function<LOGIN_RESPONSE *(char *, char *)> *)task->getCallback();
            // 获取业务请求数据
            LOGIN * data = (LOGIN *)task->getArgs();
            // 调用业务逻辑函数
            LOGIN_RESPONSE *response = (*cb)(data->account, data->password);

            // 封装响应数据到IO线程
            Header *header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_LOGIN_RESPONSE;
            header->length = sizeof(LOGIN_RESPONSE);

            // 回调函数
            std::function<void()> cb1 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)header, sizeof(Header));
            std::function<void()> cb2 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)response, sizeof(LOGIN_RESPONSE));

            while (!CAS(&this->flag, 0, 1)) ;

            // 线程安全下添加
            conn->getEventLoop()->io_cb_queue.push_back(cb1);
            conn->getEventLoop()->io_cb_queue.push_back(cb2);

            CAS(&this->flag, 1, 0);
        }
        break;
        case USER_GET_FRIEND_LIST: // 协议相关，耦合读较高
        {
            // 获取业务处理回调函数
            std::function<RESPONSE_FRIEND_LIST *(char *)> *cb = (std::function<RESPONSE_FRIEND_LIST *(char *)> *)task->getCallback();
            // 获取业务请求数据
            GET_FRIEND_LIST * data = (GET_FRIEND_LIST *)task->getArgs();
            // 调用业务逻辑函数
            RESPONSE_FRIEND_LIST *response = (*cb)(data->account);

            // 封装响应数据到IO线程
            Header *header = (Header *)malloc(sizeof(Header));
            header->type = SERVER_RESPONSE_FRIEND_LIST;
            header->length = sizeof(RESPONSE_FRIEND_LIST);

            // 回调函数
            std::function<void()> cb1 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)header, sizeof(Header));
            std::function<void()> cb2 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)response, sizeof(RESPONSE_FRIEND_LIST));
            // 协议定义的数据，有附加数据，也要发送

            std::function<void()> cb3 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)response->array, sizeof(FriendInfo)*response->friend_num);

            while (!CAS(&this->flag, 0, 1)) ;

            // 线程安全下添加
            conn->getEventLoop()->io_cb_queue.push_back(cb1);
            conn->getEventLoop()->io_cb_queue.push_back(cb2);
            conn->getEventLoop()->io_cb_queue.push_back(cb3);

            CAS(&this->flag, 1, 0);
        }
        break;
        case CHART_ONE_INTERFACE:
        {
            // 获取业务处理回调函数
            std::function<CHART_ONE *(CHART_ONE *)> *cb = (std::function<CHART_ONE *(CHART_ONE *)> *)task->getCallback();
            // 获取业务请求数据
            CHART_ONE * data = (CHART_ONE *)task->getArgs();
            // 调用业务逻辑函数
            CHART_ONE *response = (*cb)(data);

            // 封装响应数据到IO线程
            Header *header = (Header *)malloc(sizeof(Header));
            header->type = CHART_ONE_INTERFACE;
            header->length = sizeof(CHART_ONE);

            // 回调函数
            std::function<void()> cb1 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)header, sizeof(Header));
            std::function<void()> cb2 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)response, sizeof(CHART_ONE));
            // 协议定义的数据，有附加数据，也要发送

            while (!CAS(&this->flag, 0, 1)) ;

            // 线程安全下添加
            conn->getEventLoop()->io_cb_queue.push_back(cb1);
            conn->getEventLoop()->io_cb_queue.push_back(cb2);

            CAS(&this->flag, 1, 0);
        }
        break;
        case USER_GET_CHART_RECORD:
        {
            // 获取业务处理回调函数
            std::function<SERV_CHART_RECORD *(char *, char *, unsigned long long)> *cb = (std::function<SERV_CHART_RECORD *(char *, char *, unsigned long long)> *)task->getCallback();
            // 获取业务请求数据
            GET_CHART_RECORD * data = (GET_CHART_RECORD *)task->getArgs();
            // 调用业务逻辑函数
            SERV_CHART_RECORD *response = (*cb)(data->sender, data->recver, data->num);

            // 封装响应数据到IO线程
            Header *header = (Header *)malloc(sizeof(Header));
            header->type = CHART_ONE_INTERFACE;
            header->length = sizeof(SERV_CHART_RECORD);

            // 回调函数
            std::function<void()> cb1 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)header, sizeof(Header));
            std::function<void()> cb2 = std::bind(&TcpConnection::onWriteMsg, *task->getConn(), (char *)response, sizeof(SERV_CHART_RECORD));
            // 协议定义的数据，有附加数据，也要发送

            while (!CAS(&this->flag, 0, 1)) ;

            // 线程安全下添加
            conn->getEventLoop()->io_cb_queue.push_back(cb1);
            conn->getEventLoop()->io_cb_queue.push_back(cb2);

            CAS(&this->flag, 1, 0);
        }
        break;
        default:
        break;
    }

    // 唤醒对应的EventLoop的eventfd
    uint64_t one = 1;
    write(conn->getEventLoop()->getEfd(), &one, sizeof(one));
}

// 线程池预热到打满核心线程
void Threadpool::warm_up(ThreadTask *task)
{
    if (curAliveThreadNUm < coreThreadNum)
    {
        ThreadArgs * arg = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        arg->pool = this;
        arg->task = task;
        pthread_create(&coreThreads[coreThreadNum], NULL, worker, arg);
        // 分离线程
        pthread_detach(coreThreads[coreThreadNum++]);
    }
    
}

bool Threadpool::isFull()
{
    return task_queue.size() >= maxQueueSize;
}

bool Threadpool::isEmpty()
{
    return task_queue.size() == 0;
}

// 生产者向线程池任务队列中添加任务
void Threadpool::produceTask(ThreadTask *task)
{
    // 若核心线程池未打满，直接创建线程执行
    if (curAliveThreadNUm < coreThreadNum)
    {
        warm_up(task);
    }
    else // 将人物添加到任务队列中
    {
        this->locker->lock();

        if (isFull()) // 任务队列满，阻塞
        {
            // 条件变量等待
            this->notFull->wait();
        }
        task_queue.push_back(task);
        // 唤醒消费者
        this->notEmpty->signal();

        this->locker->unlock();
    }
}

// 消费者消费任务
ThreadTask * Threadpool::consumeTask()
{
    // 访问任务队列
    this->locker->lock();

    if (isEmpty())
    {
        this->notEmpty->wait();
    }

    ThreadTask * task = NULL;
    if (!isEmpty())
    {
        // 取任务执行
        task = task_queue.front();
        task_queue.pop_front();

        // 唤醒生产者
        notFull->signal();
    }
    
    return task;
}