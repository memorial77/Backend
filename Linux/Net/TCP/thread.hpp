#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <functional>
#include <pthread.h>

// 定义一个函数指针类型，用于线程执行的函数
typedef std::function<void *(void *)> func_t;
const int BUDDER_NUM = 1024; // 定义一个常量，用于生成线程名字的缓冲区大小

class Thread
{
private:
    // 静态成员函数，用作pthread_create的启动函数
    // 需要是静态的，因为非静态成员函数隐含this指针作为第一个参数，而pthread_create只接受单个参数的函数
    static void *start_routine(void *args) // 类内成员，有缺省参数！
    {
        Thread *_this = static_cast<Thread *>(args); // 将void*类型的参数转换回Thread*类型
        return _this->callback();                    // 调用实例的callback方法
    }

public:
    Thread()
    {
        char namebuffer[BUDDER_NUM];                                       // 创建一个字符数组作为线程名的缓冲区
        snprintf(namebuffer, sizeof namebuffer, "thread-%d", threadnum++); // 格式化线程名并存储在namebuffer中
        name_ = namebuffer;                                                // 将格式化后的线程名赋值给成员变量name_
    }

    // 启动线程的方法
    void start(func_t func, void *args = nullptr)
    {
        func_ = func;                                                  // 设置线程要执行的函数
        args_ = args;                                                  // 设置线程函数的参数
        int ret = pthread_create(&tid_, nullptr, start_routine, this); // 创建线程
        assert(ret == 0);                                              // 断言线程创建成功
        (void)ret;                                                     // 避免编译器警告
    }

    // 等待线程结束的方法
    void join()
    {
        int ret = pthread_join(tid_, nullptr); // 等待线程结束
        assert(ret == 0);                      // 断言线程成功结束
        (void)ret;                             // 避免编译器警告
    }

    // 获取线程名称
    std::string threadname() { return name_; }

    // 析构函数
    ~Thread() {}

    // 线程执行的回调函数
    void *callback() { return func_(args_); } // 调用func_函数指针指向的函数

private:
    std::string name_; // 线程名
    func_t func_;      // 线程执行函数
    void *args_;       // 线程函数的参数
    pthread_t tid_;    // 线程ID

    static int threadnum; // 静态成员变量，用于生成线程名
};
int Thread::threadnum = 1; // 初始化静态成员变量threadnum