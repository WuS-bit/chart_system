#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

#include <pthread.h>

class MutexLock
{
public:
    pthread_mutex_t m_mutex;
    MutexLock();
    ~MutexLock();

    bool lock();
    bool unlock();
};

MutexLock::MutexLock()
{
    pthread_mutex_init(&m_mutex, NULL);
}

MutexLock::~MutexLock()
{
    pthread_mutex_destroy(&m_mutex);
}

bool MutexLock::lock()
{
    return pthread_mutex_lock(&m_mutex);
}

bool MutexLock::unlock()
{
    return pthread_mutex_unlock(&m_mutex);
}

#endif