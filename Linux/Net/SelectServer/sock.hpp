#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.hpp"

class Sock
{
public:
    static int Socket()
    {
        // 创建套接字
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            log_msg(ERROR, "create socket failed!");
            exit(1);
        }
        std::string msg = "create socket success! sockfd: " + std::to_string(sockfd);
        log_msg(INFO, msg);

        int opt = 1;
        // 设置套接字选项
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        {
            log_msg(ERROR, "set socket option failed!");
            exit(1);
        }
        log_msg(INFO, "set socket option success!");

        return sockfd;
    }

    static void Bind(int sockfd, const std::string &ip, int port)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

        // 绑定套接字
        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            log_msg(ERROR, "bind socket failed!");
            exit(1);
        }
        std::string msg = "bind socket success! IP: " + ip + "，port: " + std::to_string(port);
        log_msg(INFO, msg);
    }

    static void Listen(int sockfd, int backlog)
    {
        // 监听套接字
        if (listen(sockfd, backlog) == -1)
        {
            log_msg(ERROR, "listen socket failed!");
            exit(1);
        }
        std::string msg = "listen socket success! backlog: " + std::to_string(backlog);
        log_msg(INFO, msg);
    }

    static int Accept(int listen_sock, std::string &client_ip, uint16_t &client_port)
    {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        // 接受连接
        int client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &len);
        if (client_sock == -1)
        {
            log_msg(ERROR, "accept socket failed!");
            exit(1);
        }

        client_ip = inet_ntoa(client_addr.sin_addr);
        client_port = ntohs(client_addr.sin_port);
        log_msg(INFO, "accept socket success! client_ip: " + client_ip + "，client_port: " + std::to_string(client_port));

        return client_sock;
    }
};