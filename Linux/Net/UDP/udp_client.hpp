#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class UdpClient
{
private:
    std::string server_ip_;         // 服务器 IP
    uint16_t server_port_;          // 服务器端口
    int sockfd;                     // 套接字文件描述符
    struct sockaddr_in server_addr; // 服务器地址结构

public:
    UdpClient(const std::string &ip, uint16_t port)
        : server_ip_(ip), server_port_(port), sockfd(-1)
    {
        // 构造函数
    }

    ~UdpClient()
    {
        // 析构函数
        close(sockfd); // 关闭套接字
    }

    void init()
    {
        // 创建套接字
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            std::cerr << "创建套接字错误\n";
            exit(1);
        }

        // 设置服务器地址
        bzero(&server_addr, sizeof(server_addr));                    // 清空服务器地址结构
        server_addr.sin_family = AF_INET;                            // 使用IPv4地址
        server_addr.sin_port = htons(server_port_);                  // 设置端口号
        server_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str()); // 设置IP地址
    }

    void start()
    {
        // 向服务器发送数据
        std::string message;
        while (true)
        {
            std::cout << "输入要发送的消息（输入 'exit' 退出）: ";
            std::getline(std::cin, message); // 从用户获取消息
            if (message == "exit")
                break;

            if (sendto(sockfd, message.c_str(), message.length(), 0,
                       (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            {
                // 发送消息错误
                std::cerr << "发送消息错误\n";
                break;
            }

            // 从服务器接收数据
            char buf[1024];
            bzero(buf, sizeof(buf)); // 清空缓冲区
            if (recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL) < 0)
            {
                // 接收数据错误
                std::cerr << "接收数据错误\n";
                break;
            }
            // 打印接收到的数据
            std::cout << buf;
        }
    }
};