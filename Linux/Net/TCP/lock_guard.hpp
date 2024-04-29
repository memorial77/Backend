#pragma once

#include <iostream>
#include <pthread.h>

class Mutex
{
public:
    // 构造函数，可选地接受一个pthread_mutex_t指针
    Mutex(pthread_mutex_t *lock_p = nullptr) : lock_p_(lock_p) {}

    // 加锁方法
    void lock()
    {
        if (lock_p_) // 如果有指定的互斥锁指针，则对其加锁
            pthread_mutex_lock(lock_p_);
    }
    // 解锁方法
    void unlock()
    {
        if (lock_p_) // 如果有指定的互斥锁指针，则对其解锁
            pthread_mutex_unlock(lock_p_);
    }

    // 析构函数
    ~Mutex() {}

private:
    pthread_mutex_t *lock_p_; // 指向互斥锁的指针
};

// LockGuard类用于自动管理Mutex的加锁和解锁
class LockGuard
{
public:
    // 构造函数，接受一个pthread_mutex_t指针，并创建一个Mutex对象，随即加锁
    LockGuard(pthread_mutex_t *mutex) : mutex_(mutex) { mutex_.lock(); }
    
    // 析构函数，在对象生命周期结束时自动解锁
    ~LockGuard() { mutex_.unlock(); }

private:
    Mutex mutex_; // Mutex对象，用于管理互斥锁
};