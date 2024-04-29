#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <functional>
#include <unistd.h>
#include "log.hpp"

// 任务类
class Task
{
    using func_t = std::function<void(int, void *)>; // 定义函数类型

public:
    Task() {} // 默认构造函数

    Task(int sock, func_t func, void *this_) : sock_(sock), callback_(func), this_(this_) {} // 构造函数，初始化套接字和回调函数

    void operator()() { callback_(sock_, this_); } // 调用回调函数，执行任务

private:
    int sock_;        // 套接字描述符
    func_t callback_; // 回调函数
    void *this_;      // this指针
};

// // 服务端I/O处理函数
// void serviceIO(int sock)
// {
//     char buffer[1024]; // 数据缓冲区
//     while (true)
//     {
//         ssize_t read_bytes = read(sock, buffer, sizeof(buffer) - 1); // 从sock读取数据
//         if (read_bytes > 0)
//         {
//             // 如果读取到数据，将数据视为字符串，并在末尾添加终止符
//             buffer[read_bytes] = '\0';
//             std::cout << "recv message: " << buffer << std::endl; // 打印接收到的消息

//             std::string outbuffer = buffer;
//             outbuffer += " server[echo]"; // 在消息后添加" server[echo]"

//             write(sock, outbuffer.c_str(), outbuffer.size()); // 将修改后的消息发送回客户端
//         }
//         else if (read_bytes == 0)
//         {
//             // 如果读取到的数据长度为0，表示客户端已关闭连接
//             log_msg(INFO, "client close, sock: " + std::to_string(sock)); // 记录日志
//             break;
//         }
//     }
//     close(sock); // 关闭套接字
// }
