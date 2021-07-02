#ifndef THREADPOOL_H
#define THREADPOOL_H

#define CPU_NUM 16
#define MAXQUEUESIZE 100000

#ifndef CAS
#define CAS __sync_bool_compare_and_swap
#endif


#include <pthread.h>
#include <deque>
#include <functional>

#include "ThreadTask.h"
#include "../util/MutexLock.h"
#include "../util/Condition.h"

// 将线程池业务处理好后的数据交回给从Reactor线程进行IO，实现方式是，利用std::function将回调函数添加到相应EventLoop的队列中（CAS保护）


using namespace std;

class ThreadTask;

// typedef struct TaskNode
// {
//     ThreadTask * task;
//     TaskNode * next;
// }TaskNode;

typedef struct ThreadArgs
{
    Threadpool *pool;
    ThreadTask *task;
} ThreadArgs;


class Threadpool
{
private:

    std::deque<ThreadTask *> task_queue;    // 线程池任务队列
    int maxQueueSize;
    MutexLock *locker;
    Condition *notEmpty;
    Condition *notFull;

    // 核心线程
    int coreThreadNum;
    int curAliveThreadNUm; // 活跃线程数
    pthread_t *coreThreads;


public:
    Threadpool(int threadNums, int maxQueueSize);
    ~Threadpool();

    void warm_up(ThreadTask *task);

    bool isFull();
    bool isEmpty();

    void produceTask(ThreadTask *task);
    ThreadTask * consumeTask();

    void excuteTask(ThreadTask *task);


    // 线程池需要将IO都转换到 EventLoop线程中，利用，std::function，需要使用同步机制，利用全局变量CAS实现
    volatile int64_t flag;
};


Threadpool * pool = new Threadpool(CPU_NUM, MAXQUEUESIZE);

#endif