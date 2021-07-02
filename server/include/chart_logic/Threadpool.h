#ifndef THREADPOOL_H
#define THREADPOOL_H

#define CPU_NUM 16
#define MAXQUEUESIZE 100000

#include <pthread.h>
#include <deque>

#include "ThreadTask.h"
#include "../util/MutexLock.h"
#include "../util/Condition.h"


using namespace std;

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
};


Threadpool * pool = new Threadpool(CPU_NUM, MAXQUEUESIZE);

#endif