#pragma once

#include "thread.hpp"
#include "lock.hpp"
#include "cal_task.hpp"
#include "event_task.hpp"
#include <vector>
#include <queue>
#include <unistd.h>

const int default_thread_count = 5;

template <class T>
class ThreadPool;

template <class T>
class ThreadData
{
public:
    ThreadData(ThreadPool<T> *p_pool, Thread *p_thread) : thread_pool_p_(p_pool), thread_p_(p_thread) {}

    ThreadPool<T> *thread_pool_p_; // 线程池指针
    Thread *thread_p_;             // 线程指针
};

template <class T>
class ThreadPool
{
public:
    // 获取单例对象接口（懒汉模式线程安全版本）
    static ThreadPool<T> *get_instance(int thread_nums = default_thread_count)
    {
        if (instance_ == nullptr)
        {
            // 实例还未创建则加锁
            LockGuard lock(&i_mutex_);
            // 防止多线程出现多次进入问题
            if (instance_ == nullptr)
                instance_ = new ThreadPool(thread_nums);

            return instance_;
        }
    }

    // 启动线程池
    void run()
    {
        for (const auto &thread : threads_)
        {
            ThreadData<T> *td_p = new ThreadData<T>(this, thread);
            thread->start(handler_task, (void *)td_p);
            std::cout << thread->thread_name() << " starting...\n";
        }
    }

    // 放置任务接口
    void push(const T &task)
    {
        // 访问任务队列之前加锁
        LockGuard lock(&mutex_);
        task_queue_.push(task);
        thread_signal(); // 唤醒对应阻塞线程
    }

    // 析构函数
    ~ThreadPool()
    {
        // 销毁互斥锁和条件变量
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);

        // 释放开辟的线程空间
        for (const auto &thread : threads_)
            delete thread;
    }

    ThreadPool<T> *operator=(const ThreadPool<T> &other) = delete;
    ThreadPool(const ThreadPool<T> &other) = delete;

public:
    // 判断任务队列是否为空
    bool is_queue_empty() { return task_queue_.empty(); }
    // 阻塞线程接口
    void thread_wait() { pthread_cond_wait(&cond_, &mutex_); }
    // 唤醒线程接口
    void thread_signal() { pthread_cond_signal(&cond_); }
    // 取出任务接口
    T get_task()
    {
        T task = task_queue_.front();
        task_queue_.pop();
        return task;
    }

private:
    // 构造函数私有化
    ThreadPool(int thread_nums = default_thread_count) : thread_nums_(thread_nums)
    {
        // 初始化任务队列互斥锁和同步变量
        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&cond_, nullptr);
        // 创建初始线程
        for (int i = 0; i < thread_nums_; i++)
            threads_.push_back(new Thread());
    }

    // 池内线程执行函数
    static void *handler_task(void *arg)
    {
        // 将ThreadData指针通过参数传递
        ThreadData<T> *td_p = static_cast<ThreadData<T> *>(arg);

        ThreadPool<T> *thread_pool = td_p->thread_pool_p_;
        Thread *thread_p = td_p->thread_p_;
        while (true)
        {
            T task;
            {
                // 访问任务队列之前加锁
                LockGuard lock(&thread_pool->mutex_);
                while (thread_pool->task_queue_.empty()) // 对列为空则阻塞
                    thread_pool->thread_wait();

                task = thread_pool->get_task(); // 本质：将公共任务队列中的任务取至自身独立栈中进行处理
            }
            // 将任务放置外部处理防止无效占用锁时间，否则则是串行执行任务
            std::cout << thread_p->thread_name()
                      << " do: " << task.toTaskString()
                      << " result: " << task() << std::endl;
            // task();
        }
        delete td_p;
        return nullptr;
    }

private:
    int thread_nums_ = 5;           // 池内线程数
    std::vector<Thread *> threads_; // 存储池内线程
    std::queue<T> task_queue_;      // 池内任务队列
    pthread_mutex_t mutex_;         // 互斥锁
    pthread_cond_t cond_;           // 同步变量

    static ThreadPool<T> *instance_; // 单例静态成员变量
    static pthread_mutex_t i_mutex_; // instance_互斥锁
};

// 类外初始化静态成员变量
template <class T>
ThreadPool<T> *ThreadPool<T>::instance_ = nullptr;

template <class T>
pthread_mutex_t ThreadPool<T>::i_mutex_ = PTHREAD_MUTEX_INITIALIZER;