#pragma once

#include "thread.hpp"
#include "lock_guard.hpp"
#include "log.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <pthread.h>
#include <unistd.h>

const int gnum = 10; // 默认的线程池中线程的数量

template <class T>
class ThreadPool;

// 线程数据类
template <class T>
class ThreadData
{
public:
    ThreadPool<T> *threadpool_p_; // 线程池指针
    std::string name_;            // 线程名称

public:
    ThreadData(ThreadPool<T> *tp, const std::string &name) : threadpool_p_(tp), name_(name) {}
};

// 线程池类
template <class T>
class ThreadPool
{
public:
    // 锁定任务队列
    void lock_queue() { pthread_mutex_lock(&mutex_); }
    // 解锁任务队列
    void unlock_queue() { pthread_mutex_unlock(&mutex_); }
    // 检查任务队列是否为空
    bool is_queue_empty() { return task_queue_.empty(); }
    // 线程等待条件变量
    void thread_wait() { pthread_cond_wait(&cond_, &mutex_); }

    // 从任务队列中取出一个任务
    T pop()
    {
        T task = task_queue_.front(); // 获取队列前端的任务
        task_queue_.pop();            // 从队列中移除该任务
        return task;
    }
    // 获取互斥锁的指针
    pthread_mutex_t *mutex()
    {
        return &mutex_;
    }

public:
    // 启动线程池中的所有线程
    void run()
    {
        for (const auto &thread : threads_)
        {
            ThreadData<T> *td = new ThreadData<T>(this, thread->threadname()); // 创建线程数据
            thread->start(handlerTask, td);                                    // 启动线程
            // log_msg(INFO, thread->threadname() + " start ...");
        }
    }
    // 向任务队列中添加一个任务
    void push(const T &in)
    {
        LockGuard lockguard(&mutex_); // 使用锁保护任务队列
        task_queue_.push(in);         // 将任务添加到队列中
        pthread_cond_signal(&cond_);  // 通知等待的线程有新任务到来
    }

    // 析构函数用以清理资源
    ~ThreadPool()
    {
        pthread_mutex_destroy(&mutex_); // 销毁互斥锁
        pthread_cond_destroy(&cond_);   // 销毁条件变量

        for (const auto &thread : threads_) // 删除所有线程对象
            delete thread;
    }

    // 获取线程池的单例
    static ThreadPool<T> *get_instance()
    {
        if (nullptr == tp_) // 双重检查锁定确保线程安全
        {
            sing_lock_.lock(); // 锁定
            if (nullptr == tp_)
                tp_ = new ThreadPool<T>(); // 创建线程池实例
            sing_lock_.unlock();           // 解锁
        }
        return tp_;
    }

private:
    int thread_num_;                // 线程池中线程的数量
    std::vector<Thread *> threads_; // 存储线程对象的向量
    std::queue<T> task_queue_;      // 任务队列
    pthread_mutex_t mutex_;         // 互斥锁
    pthread_cond_t cond_;           // 条件变量

    static ThreadPool<T> *tp_;    // 线程池的单例指针
    static std::mutex sing_lock_; // 用于单例的互斥锁

private:
    // 构造函数私有化
    ThreadPool(const int &num = gnum) : thread_num_(num)
    {
        pthread_mutex_init(&mutex_, nullptr); // 初始化互斥锁
        pthread_cond_init(&cond_, nullptr);   // 初始化条件变量

        for (int i = 0; i < thread_num_; i++)
            threads_.push_back(new Thread()); // 创建线程对象并添加到向量中
    }

    // 静态成员函数，用作线程的处理函数
    static void *handlerTask(void *args)
    {
        ThreadData<T> *td = (ThreadData<T> *)args; // 从参数中获取线程数据
        while (true)
        {
            T task;
            // 代码块内部使用锁避免锁的持有时间过长
            {
                LockGuard lockguard(td->threadpool_p_->mutex()); // 使用锁保护任务队列
                while (td->threadpool_p_->is_queue_empty())      // 如果任务队列为空，则等待
                    td->threadpool_p_->thread_wait();

                task = td->threadpool_p_->pop(); // 从任务队列中取出一个任务
            }

            task(); // 代码块外部执行任务
        }
        delete td; // 删除线程数据
        return nullptr;
    }

    ThreadPool(const ThreadPool &) = delete;     // 禁止拷贝构造函数
    void operator=(const ThreadPool &) = delete; // 禁止赋值操作符
};

template <class T>
ThreadPool<T> *ThreadPool<T>::tp_ = nullptr; // 初始化单例指针

template <class T>
std::mutex ThreadPool<T>::sing_lock_; // 初始化单例的互斥锁