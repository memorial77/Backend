#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <strings.h>

class UdpServer
{
public:
    // 构造函数
    UdpServer(uint16_t port, const std::string &ip = "0.0.0.0")
        : port_(port), ip_(ip), sockfd_(-1)
    {
        // 构造函数中的代码
    }

    // 析构函数
    ~UdpServer()
    {
        close(sockfd_); // 关闭套接字
    }

    // 初始化服务器
    void init()
    {
        // 1.创建套接字
        // AF_INET: IPv4 或 AF_INET6: IPv6
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ < 0)
        {
            std::cerr << "socket 错误" << std::endl; // 打印错误信息
            exit(1);
        }

        // 2.绑定套接字
        struct sockaddr_in server_addr;                       // 服务器地址
        bzero(&server_addr, sizeof(server_addr));             // 清空服务器地址
        server_addr.sin_family = AF_INET;                     // IPv4
        server_addr.sin_port = htons(port_);                  // 端口号
        server_addr.sin_addr.s_addr = inet_addr(ip_.c_str()); // IP地址
        // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // 接受任何IP地址

        if (bind(sockfd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            std::cerr << "bind 错误" << std::endl;
            exit(2);
        }

        // 打印服务器信息
        std::cout << "服务器正在运行在 " << ip_ << ":" << port_ << std::endl;
    }

    // 启动服务器
    void start()
    {
        char buf[1024];                 // 缓冲区
        struct sockaddr_in client_addr; // 客户端地址
        socklen_t len = sizeof(client_addr);

        // 3.接收数据
        while (1)
        {
            memset(buf, 0, sizeof(buf)); // 清空缓冲区
            // 从客户端接收数据，留出空间用于空终止符
            ssize_t recv_size = recvfrom(sockfd_, buf, sizeof(buf) - 1, 0,
                                         (struct sockaddr *)&client_addr, &len);
            if (recv_size < 0)
            {
                std::cerr << "recvfrom 错误" << std::endl;
                continue; // 继续下一次循环
            }
            // 手动添加接收数据的空终止符
            buf[recv_size] = '\0';

            std::string output; // 命令的输出
            // 检查危险命令，如 rm, mv, cp
            char *dangerous_cmd = NULL;
            if ((dangerous_cmd = strcasestr(buf, "rm")) != NULL ||
                (dangerous_cmd = strcasestr(buf, "mv")) != NULL ||
                (dangerous_cmd = strcasestr(buf, "cp")) != NULL)
            {
                // 如果发现危险命令，不执行
                std::cerr << "检测到危险命令: " << dangerous_cmd << std::endl;
                output = "检测到危险命令: " + std::string(dangerous_cmd) + "\n";
            }
            else
            {
                FILE *fp = popen(buf, "r");
                if (fp == NULL)
                {
                    std::cerr << "popen 错误" << std::endl;
                    break;
                }

                // 逐行读取命令输出
                char temp[1024];
                while (fgets(temp, sizeof(temp), fp) != NULL)
                    output += temp;
                pclose(fp);
            }

            // 如果客户端发送 "exit"，服务器将退出
            if (strcmp(buf, "exit") == 0)
                break;

            // 将输出发送回客户端
            if (sendto(sockfd_, output.c_str(), output.length(), 0,
                       (struct sockaddr *)&client_addr, len) < 0)
            {
                std::cerr << "sendto 错误" << std::endl;
                break;
            }
        }
    }

private:
    uint16_t port_;  // 端口号
    std::string ip_; // IP地址
    int sockfd_;     // 套接字文件描述符
};