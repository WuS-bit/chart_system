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

inline MutexLock::MutexLock()
{
    pthread_mutex_init(&m_mutex, NULL);
}

inline MutexLock::~MutexLock()
{
    pthread_mutex_destroy(&m_mutex);
}

inline bool MutexLock::lock()
{
    return pthread_mutex_lock(&m_mutex);
}

inline bool MutexLock::unlock()
{
    return pthread_mutex_unlock(&m_mutex);
}

#endif