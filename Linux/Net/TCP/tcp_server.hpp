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

class TcpServer
{
public:
    // 构造函数
    TcpServer(uint16_t port, std::string ip = "0.0.0.0") : port_(port), ip_(ip), listen_fd_(-1) {}

    // 析构函数
    ~TcpServer()
    {
        if (listen_fd_ != -1)
            close(listen_fd_);
    }

    // 初始化服务器
    void init()
    {
        // 1.创建监听套接字
        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd_ == -1)
        {
            std::cerr << "create socket error: " << strerror(errno) << std::endl;
            exit(1);
        }

        // 2.绑定
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_);
        server_addr.sin_addr.s_addr = inet_addr(ip_.c_str());
        if (bind(listen_fd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            std::cerr << "bind error: " << strerror(errno) << std::endl;
            exit(1);
        }

        // 3.监听
        if (listen(listen_fd_, 1024) < 0)
        {
            std::cerr << "listen error: " << strerror(errno) << std::endl;
            exit(1);
        }
    }

    // 启动服务器
    void start()
    {
        // 1.初始化服务器
        init();

        while (true)
        {
            // 2.接受客户端连接
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int conn_fd = accept(listen_fd_, (struct sockaddr *)&client_addr, &client_addr_len);
            if (conn_fd < 0)
            {
                std::cerr << "accept error: " << strerror(errno) << std::endl;
                exit(1);
            }

            // 创建新线程处理客户端连接
            std::thread(&TcpServer::handle_client, this, conn_fd, client_addr).detach();
        }
    }

private:
    uint16_t port_;  // 端口
    std::string ip_; // ip地址
    int listen_fd_;  // 监听套接字

    void handle_client(int conn_fd, struct sockaddr_in client_addr)
    {
        // 获取客户端信息
        uint16_t client_port = ntohs(client_addr.sin_port);
        std::string client_ip = inet_ntoa(client_addr.sin_addr);

        std::cout << "连接成功: " << client_ip << ":" << client_port << std::endl;

        while(true)
        {
            char buf[1024] = {0};
            if (recv(conn_fd, buf, sizeof(buf), 0) < 0)
            {
                std::cerr << "recv error: " << strerror(errno) << std::endl;
                break;
            }

            std::cout << "接收到客户端的消息: " << buf << std::endl;

            if (send(conn_fd, buf, strlen(buf), 0) < 0)
            {
                std::cerr << "send error: " << strerror(errno) << std::endl;
                break;
            }
        }
        // 关闭连接
        close(conn_fd);
    }
};