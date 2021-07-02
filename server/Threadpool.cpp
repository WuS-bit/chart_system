#include "include/chart_logic/Threadpool.h"

Threadpool::Threadpool(int threadNums, int maxQueueSize)
{
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

    while (1)
    {
        pool->excuteTask(task);

        delete task;
        task = pool->consumeTask();
    }
    
}

void Threadpool::excuteTask(ThreadTask *task)
{
    // 分析任务体，执行对应业务逻辑函数
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