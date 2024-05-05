#pragma once

#include <poll.h>
#include <functional>
#include "sock.hpp"

static const std::string DEFAULT_IP = "0.0.0.0"; // 默认IP地址
static const uint16_t DEFAULT_PORT = 8888;       // 默认端口号
static const int FD_SIZE = 1024;                 // 文件描述符集合大小

using func_t = std::function<std::string(const std::string &)>; // 函数指针类型

class PollServer
{
public:
    // 构造函数
    PollServer(func_t func, std::string ip = DEFAULT_IP, uint16_t port = DEFAULT_PORT)
        : ip_(ip), port_(port), listen_sock_(-1), fds_(nullptr), func_(func)
    {
    }

    // 析构函数
    ~PollServer()
    {
        if (listen_sock_ != -1) // 关闭监听套接字
            close(listen_sock_);

        if (fds_) //  释放pollfd结构体数组
            delete[] fds_;
    }

    void reset(int index)
    {
        fds_[index].fd = -1;
        fds_[index].events = 0;
        fds_[index].revents = 0;
    }

    void init()
    {
        // 创建套接字
        listen_sock_ = Sock::Socket();
        // 绑定套接字
        Sock::Bind(listen_sock_, ip_, port_);
        // 监听套接字
        Sock::Listen(listen_sock_, 128);
        // 创建pollfd结构体数组
        fds_ = new pollfd[FD_SIZE];
        // 初始化pollfd结构体数组
        for (int i = 0; i < FD_SIZE; i++)
            reset(i);

        fds_[0].fd = listen_sock_; // 将监听套接字添加到数组中
        fds_[0].events = POLLIN;   // 监听读事件
    }

    void start()
    {
        while (true)
        {
            int timeout = 100; // 超时时间

            int ret = poll(fds_, FD_SIZE, timeout); // 监听事件

            if (ret < 0) // 出错
            {
                log_msg(ERROR, "poll error");
                return;
            }
            else if (ret == 0) // 超时
            {
                log_msg(INFO, "poll timeout");
                continue;
            }
            else // 读事件就绪
            {
                log_msg(INFO, "have event ready!");
                handler_read_event();
            }
        }
    }

    void handler_read_event()
    {
        for (int i = 0; i < FD_SIZE; i++)
        {
            if (fds_[i].fd == -1) // 过滤掉非法的文件描述符
                continue;

            if (fds_[i].revents & POLLIN) // 读事件就绪
            {
                if (fds_[i].fd == listen_sock_)
                    // 处理新连接
                    handler_accept_event();
                else
                    // 处理已连接客户端
                    handler_client_event(i);
            }
        }
    }

    void handler_accept_event()
    {
        std::string client_ip;
        uint16_t client_port;
        int client_sock = Sock::Accept(listen_sock_, client_ip, client_port);

        // 将客户端套接字添加到数组中
        int count = 0;
        for (; count < FD_SIZE; ++count)
        {
            if (fds_[count].fd == -1)
            {
                fds_[count].fd = client_sock;
                fds_[count].events = POLLIN;
                break;
            }
        }

        // 客户端连接数超过最大值
        if (count == FD_SIZE)
        {
            close(client_sock);
            log_msg(ERROR, "too many clients!");
        }
    }

    void handler_client_event(int index)
    {
        char buf[1024] = {0};
        int ret = recv(fds_[index].fd, buf, sizeof(buf), 0);
        if (ret < 0)
        {
            log_msg(ERROR, "recv error");
            close(fds_[index].fd);
            reset(index);
        }
        else if (ret == 0)
        {
            log_msg(INFO, "client closed");
            close(fds_[index].fd);
            reset(index);
        }
        else
        {
            log_msg(INFO, "recv success! data: " + std::string(buf));
            // send(fds_[index], buf, ret, 0);
            // 加工数据
            std::string data = func_(buf);
            send(fds_[index].fd, data.c_str(), data.size(), 0);
        }
    }

private:
    int listen_sock_;    // 监听套接字
    std::string ip_;     // IP地址
    uint16_t port_;      // 端口号
    struct pollfd *fds_; // pollfd结构体数组
    func_t func_;        // callback函数
};