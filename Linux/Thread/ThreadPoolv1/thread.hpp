#pragma once

#include <pthread.h>
#include <functional>
#include <iostream>
#include <string>
#include <string.h>

class Thread
{
private:
    typedef std::function<void *(void *)> func_t;

public:
    // 线程构造函数
    Thread()
        : detached_(false)
    {
        // 构建线程名称
        char name_buffer[1024];
        snprintf(name_buffer, sizeof(name_buffer), "Thread[%d]", thread_num_++);
        name_ = name_buffer;
    }

    // 创建并启动线程
    void start(func_t func, void *arg = nullptr)
    {
        callback_ = func;
        arg_ = arg;

        // 此处无法直接使用callback_
        // 报错：不存在从 "Thread::func_t" 到 "void *(*)(void *)" 的适当转换函数
        int result = pthread_create(&tid_, nullptr, routine, (void *)this);
        if (result != 0)
        {
            // 线程创建失败，打印错误信息
            std::cerr << "Failed to create thread: "
                      << strerror(result) << std::endl;
            exit(-1);
        }
    }

    // 线程分离
    bool detach()
    {
        int result = pthread_detach(tid_);
        if (result != 0)
        {
            // 线程分离失败，打印错误信息
            std::cerr << "Fail to detach thread: "
                      << strerror(result) << std::endl;
            exit(-1);
        }
        detached_ = true;
    }

    // 获取线程名称
    std::string thread_name() { return name_; }

    // 析构函数
    ~Thread()
    {
        // 线程未分离则进行等待
        if (!detached_)
            join();
    }

private:
    // 线程调用函数
    // 要想使得线程能够调用，类内必须使用static静态函数，因为不隐式传递this指针
    static void *routine(void *arg)
    {
        Thread *this_ = static_cast<Thread *>(arg);
        // 调用实际回调函数
        return this_->callback_(this_->arg_);
    }

    // 线程等待
    // 设置为私有使得在析构时自动调用而不需要显式调用
    void join()
    {
        int result = pthread_join(tid_, nullptr);
        if (result != 0)
        {
            // 线程等待失败，打印错误信息
            std::cerr << "Failed to join thread: "
                      << strerror(result) << std::endl;
            exit(-1);
        }
    }

private:
    std::string name_;      // 线程名称
    pthread_t tid_;         // 线程id
    func_t callback_;       // 线程回调函数
    void *arg_;             // 回调函数参数
    bool detached_;         // 是否已分离
    static int thread_num_; // 线程编号
};

int Thread::thread_num_ = 1;