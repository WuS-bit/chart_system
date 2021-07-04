#ifndef CONDITION_H
#define CONDITION_H

#include "MutexLock.h"

class Condition
{
private:
    MutexLock *m_mutex;
    pthread_cond_t m_cond;
public:
    Condition(MutexLock *mutexLock);
    ~Condition();

    bool wait();
    bool signal();
};

inline Condition::Condition(MutexLock *mutexLock)
{
    this->m_mutex = mutexLock;
    pthread_cond_init(&this->m_cond, NULL);
}

inline Condition::~Condition()
{
    pthread_cond_destroy(&this->m_cond);
    if (m_mutex != NULL)
    {
        delete m_mutex;   
    }
}

inline bool Condition::wait()
{
    return pthread_cond_wait(&m_cond, &m_mutex->m_mutex) == 0;
}

inline bool Condition::signal()
{
    return pthread_cond_signal(&m_cond) == 0;
}

#endif