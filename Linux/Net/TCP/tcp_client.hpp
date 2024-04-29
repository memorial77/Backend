#pragma once

#include <iostream>
#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "log.hpp"

// TCP客户端类
class TcpClient
{
public:
    // 构造函数
    TcpClient(std::string ip, uint16_t port) : connect_fd_(-1), server_ip_(ip), server_port_(port) {}

    // 析构函数
    ~TcpClient()
    {
        // 关闭套接字
        if (connect_fd_ != -1)
            close(connect_fd_);
    }

    // 初始化客户端
    void init()
    {
        // 1.创建套接字
        connect_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (connect_fd_ == -1)
        {
            log_msg(ERROR, "socket error: " + std::string(strerror(errno)));
            exit(1);
        }
        log_msg(INFO, "connect_fd: " + std::to_string(connect_fd_));
    }

    // 启动客户端
    void start()
    {
        // 2.连接服务器
        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr)); // 清空地址结构体
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port_);
        server_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());

        if (connect(connect_fd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            log_msg(ERROR, "connect error: " + std::string(strerror(errno)));
            exit(1);
        }
        log_msg(INFO, "connected to server successfully");

        // 3.发送和接收消息
        std::string msg; // 消息缓冲区
        while (true)
        {
            std::cout << "Enter ('exit' to quit): "; // 提示用户输入消息
            std::getline(std::cin, msg);             // 使用getline代替cin >> msg;

            if (msg == "exit") // 输入exit则退出
                break;

            // 检查消息是否为空
            if (msg.empty())
            {
                std::cout << "消息不能为空，请重新输入" << std::endl;
                continue; // 跳过本次循环，不发送空消息
            }

            // 发送消息
            ssize_t send_len = send(connect_fd_, msg.c_str(), msg.length(), 0);
            if (send_len < 0)
            {
                log_msg(ERROR, "send error: " + std::string(strerror(errno)));
                break;
            }

            // 接收消息
            char buf[1024];              // 接收缓冲区
            memset(buf, 0, sizeof(buf)); // 清空缓冲区
            ssize_t recv_len = recv(connect_fd_, buf, sizeof(buf), 0);
            if (recv_len < 0)
            {
                log_msg(ERROR, "recv error: " + std::string(strerror(errno)));
                break;
            }
            else if (recv_len == 0)
            {
                log_msg(ERROR, "server closed connection");
                break;
            }

            std::cout << "server: " << buf << std::endl;
        }
    }

private:
    int connect_fd_;        // 连接套接字
    std::string server_ip_; // 服务器IP
    uint16_t server_port_;  // 服务器端口
};