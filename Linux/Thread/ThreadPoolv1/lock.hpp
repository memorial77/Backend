#pragma once

#include <pthread.h>

// 互斥锁类
class Mutex
{
public:
    // 构造函数，传入互斥锁指针
    Mutex(pthread_mutex_t *lock) : lock_(lock) {}

    // 加锁操作
    void lock()
    {
        // 如果互斥锁有效，则进行加锁操作
        if (lock_)
            pthread_mutex_lock(lock_);
    }

    // 解锁操作
    void unlock()
    {
        // 如果互斥锁有效，则进行解锁操作
        if (lock_)
            pthread_mutex_unlock(lock_);
    }

    // 析构函数，无需做特殊处理
    ~Mutex() {}

private:
    pthread_mutex_t *lock_; // 互斥锁指针
};

// 锁保护类
class LockGuard
{
public:
    // 构造函数，传入互斥锁指针，进行加锁
    LockGuard(pthread_mutex_t *mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }

    // 析构函数，在对象生命周期结束时对互斥锁进行解锁
    ~LockGuard()
    {
        mutex_.unlock();
    }

private:
    Mutex mutex_; // 互斥锁对象
};
