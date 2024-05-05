#pragma once

#include "sock.hpp"
#include <functional>

static const std::string DEFAULT_IP = "0.0.0.0"; // 默认IP地址
static const uint16_t DEFAULT_PORT = 8888;       // 默认端口号
static const int FD_SIZE = sizeof(fd_set) * 8;   // 文件描述符集合大小
static const int DEFAULT_FD = -1;                // 默认文件描述符

using func_t = std::function<std::string(const std::string &)>;

// 多路复用服务器
class SelectServer
{
public:
    // 构造函数
    SelectServer(func_t func, std::string ip = DEFAULT_IP, uint16_t port = DEFAULT_PORT)
        : ip_(ip), port_(port), listen_sock_(-1), fds_(nullptr), func_(func)
    {
    }

    // 初始化服务器
    void init()
    {
        // 创建套接字
        listen_sock_ = Sock::Socket();
        // 绑定套接字
        Sock::Bind(listen_sock_, ip_, port_);
        // 监听套接字
        Sock::Listen(listen_sock_, 128);
        // 创建客户端套接字数组
        fds_ = new int[FD_SIZE];
        // 初始化客户端套接字数组
        for (int i = 0; i < FD_SIZE; ++i)
            fds_[i] = DEFAULT_FD;

        fds_[0] = listen_sock_; // 将监听套接字添加到数组中
    }

    void start()
    {
        while (true)
        {
            fd_set rfds;    // 可读文件描述符集合
            FD_ZERO(&rfds); //  清空文件描述符集合

            int max_fd = fds_[0]; // 最大文件描述符

            for (int i = 0; i < FD_SIZE; ++i)
            {
                if (fds_[i] != DEFAULT_FD)
                    FD_SET(fds_[i], &rfds); // 将文件描述符添加到集合中

                if (fds_[i] > max_fd)
                    max_fd = fds_[i]; // 更新最大文件描述符
            }

            // 设置超时时间
            struct timeval tv;
            tv.tv_sec = 3;  //  3秒
            tv.tv_usec = 0; // 0微秒

            // 监听文件描述符集合
            int ret = select(max_fd + 1, &rfds, nullptr, nullptr, &tv);

            if (ret == -1) // 出错
            {
                log_msg(ERROR, "select error!");
                break;
            }
            else if (ret == 0) // 超时
            {
                log_msg(INFO, "select timeout!");
                continue;
            }
            else // 成功
                handler_read_event(rfds);
        }
    }

    // 处理可读事件
    void handler_read_event(fd_set &rfds)
    {
        for (int i = 0; i < FD_SIZE; ++i)
        {
            // 过滤无效文件描述符
            if (fds_[i] == DEFAULT_FD)
                continue;

            if (FD_ISSET(fds_[i], &rfds))
            {
                if (fds_[i] == listen_sock_)
                    // 处理新连接
                    handler_accept_event();
                else
                    // 处理已连接客户端
                    handler_client_event(i);
            }
        }
    }

    // 处理新连接事件
    void handler_accept_event()
    {
        std::string client_ip;
        uint16_t client_port;
        int client_sock = Sock::Accept(listen_sock_, client_ip, client_port);

        // 将客户端套接字添加到数组中
        int count = 0;
        for (; count < FD_SIZE; ++count)
        {
            if (fds_[count] == DEFAULT_FD)
            {
                fds_[count] = client_sock;
                break;
            }
        }

        // 客户端连接数超过最大值
        if (count == FD_SIZE)
        {
            log_msg(ERROR, "too many clients!");
            close(client_sock);
        }

        // print(); // 打印客户端套接字数组
    }

    // 处理客户端事件
    void handler_client_event(int index)
    {
        char buf[1024] = {0};
        int ret = recv(fds_[index], buf, sizeof(buf), 0); // 接收数据
        if (ret == -1)
        {
            log_msg(ERROR, "recv error!");
            close(fds_[index]);
            fds_[index] = DEFAULT_FD;
        }
        else if (ret == 0) // 客户端关闭
        {
            log_msg(INFO, "client close!");
            close(fds_[index]);
            fds_[index] = DEFAULT_FD;
        }
        else // 接收成功
        {
            log_msg(INFO, "recv success! data: " + std::string(buf));
            // send(fds_[index], buf, ret, 0);
            // 加工数据
            std::string data = func_(buf);
            send(fds_[index], data.c_str(), data.size(), 0);
        }
    }

    void print()
    {
        for (int i = 0; i < FD_SIZE; ++i)
        {
            if (fds_[i] != DEFAULT_FD)
                log_msg(INFO, "fds_[" + std::to_string(i) + "]: " + std::to_string(fds_[i]));
        }
    }

    // 析构函数
    ~SelectServer()
    {
        if (fds_ != nullptr)
            delete[] fds_;
        close(listen_sock_);
    }

private:
    int listen_sock_; // 监听套接字
    std::string ip_;  // IP地址
    uint16_t port_;   // 端口号
    int *fds_;        // 客户端套接字数组
    func_t func_;     // 处理函数
};