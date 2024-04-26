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
#include <thread>

class TcpClient
{
public:
    TcpClient(std::string ip, uint16_t port) : ip_(ip), port_(port), connect_fd_(-1) {}

    ~TcpClient()
    {
        if (connect_fd_ != -1)
            close(connect_fd_);
    }

    void init()
    {
        // 1.创建套接字
        connect_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (connect_fd_ == -1)
        {
            std::cerr << "create socket error: " << strerror(errno) << std::endl;
            exit(1);
        }

        // 2.连接服务器
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_);
        server_addr.sin_addr.s_addr = inet_addr(ip_.c_str());
        if (connect(connect_fd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            std::cerr << "connect error: " << strerror(errno) << std::endl;
            exit(1);
        }
    }

    void start()
    {
        init();

        std::string msg;
        while (true)
        {
            std::cout << "请输入要发送的消息: ";
            std::cin >> msg;

            if (msg == "exit")
                break;

            if (send(connect_fd_, msg.c_str(), msg.size(), 0) < 0)
            {
                std::cerr << "send error: " << strerror(errno) << std::endl;
                break;
            }

            char buf[1024] = {0};
            if (recv(connect_fd_, buf, sizeof(buf), 0) < 0)
            {
                std::cerr << "recv error: " << strerror(errno) << std::endl;
                break;
            }

            std::cout << "接收到服务器的消息: " << buf << std::endl;
        }
    }

private:
    int connect_fd_;
    std::string ip_;
    uint16_t port_;
};