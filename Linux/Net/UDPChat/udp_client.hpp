#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

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
        start_receiving(); // 启动接收线程
        std::string message;
        while (true)
        {
            std::cout << "Enter ('exit' 退出): ";
            std::getline(std::cin, message);

            // 退出
            if (message == "exit")
            {
                // 发送下线命令
                const std::string offline_cmd = "offline";
                sendto(sockfd, offline_cmd.c_str(), offline_cmd.length(), 0,
                       (struct sockaddr *)&server_addr, sizeof(server_addr));
                break;
            }

            // 发送消息
            if (sendto(sockfd, message.c_str(), message.length(), 0,
                       (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            {
                std::cerr << "发送消息错误\n";
                break;
            }
        }
    }

private:
    // 启动接收线程
    void start_receiving()
    {
        std::thread receiveThread(&UdpClient::receive_message, this);
        receiveThread.detach(); // 分离线程
    }

    void receive_message()
    {
        while (true)
        {
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            if (recvfrom(sockfd, buf, sizeof(buf) - 1, 0, NULL, NULL) > 0)
                std::cout << buf << std::endl;
            else
                break;
        }
    }
};