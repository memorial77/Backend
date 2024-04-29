#pragma once

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
#include <atomic>
#include <chrono>
#include <deque>
#include <mutex>
#include "log.hpp"
#include "thread_pool.hpp"
#include "task.hpp"

// 错误码
enum
{
    SOCKET_ERR = 1, // 创建套接字错误
    BIND_ERR,       // 绑定地址错误
    LISTEN_ERR,     // 监听错误
    ACCEPT_ERR,     // 接受连接错误
    RECV_ERR,       // 接收数据错误
    SEND_ERR        // 发送数据错误
};

static const uint16_t DEFAULT_PORT = 8080;       // 默认端口号
static const int MAX_CONNECTIONS = 100;          // 最大连接数
static const int MAX_CONNECTIONS_PER_SECOND = 5; // 每秒最大连接尝试次数

// TCP服务器类
class TcpServer
{
public:
    // 获取单例对象
    static TcpServer *get_instance()
    {
        static TcpServer *instance = new TcpServer(); // 静态局部变量，程序结束时自动释放
        return instance;
    }

    // 初始化服务器
    void init()
    {
        // 1.创建监听套接字
        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd_ == -1)
        {
            log_msg(ERROR, "socket error: " + std::string(strerror(errno)));
            exit(SOCKET_ERR);
        }
        log_msg(INFO, "socket created successfully"); // 创建套接字成功

        int optval = 1;
        setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); // 设置地址重用

        // 2.绑定地址
        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr)); // 清空地址结构体
        server_addr.sin_family = AF_INET;         // IPv4
        server_addr.sin_port = htons(DEFAULT_PORT);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(listen_fd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            log_msg(ERROR, "bind error: " + std::string(strerror(errno)));
            exit(BIND_ERR);
        }
        log_msg(INFO, "bind address successfully"); // 绑定地址成功

        // 3.监听
        if (listen(listen_fd_, MAX_CONNECTIONS) == -1)
        {
            log_msg(ERROR, "listen error: " + std::string(strerror(errno)));
            exit(LISTEN_ERR);
        }
        log_msg(INFO, "listen successfully"); // 监听成功
    }

    // 启动服务器
    void start()
    {
        log_msg(INFO, "server started, listening on port " + std::to_string(DEFAULT_PORT));

        // 4.启动线程池
        ThreadPool<Task> *tp = ThreadPool<Task>::get_instance(); // 获取线程池单例
        tp->run();                                               // 启动线程池
        log_msg(INFO, "thread pool started");                    // 线程池启动成功

        while (true)
        {
            if (!accept_new_connection()) // 拒绝连接
                continue;

            // 5.建立连接
            int conn_fd = accept_connection();
            if (conn_fd == -1) // 建立连接失败
                continue;

            // 当前连接数
            log_msg(INFO, "current connections: " + std::to_string(current_connections.load()));

            // // 创建新线程处理客户端连接(多线程处理多个客户端连接)
            // std::thread client_thread([this, conn_fd]()
            //                           { this->handle_client(conn_fd);
            //                             this->connection_closed(); }); // 使用lambda表达式
            // client_thread.detach(); // 不等待线程结束

            // 使用线程池处理客户端连接(线程池处理多个客户端连接)
            Task task(conn_fd, handle_client, this); // 创建任务对象
            tp->push(task);                          // 将任务添加到线程池
        }
    }

private:
    int listen_fd_;                                                        // 监听套接字
    std::atomic<int> current_connections{0};                               // 当前连接数
    std::deque<std::chrono::steady_clock::time_point> connection_attempts; // 连接尝试时间
    std::mutex connection_attempts_mutex;                                  // 互斥锁

private:
    // 构造函数私有化
    TcpServer(uint16_t port = DEFAULT_PORT) : listen_fd_(-1) {}

    // 禁用拷贝构造函数和赋值运算符
    TcpServer(const TcpServer &) = delete;
    TcpServer &operator=(const TcpServer &) = delete;

    // 判断是否接受新连接
    bool accept_new_connection()
    {
        std::lock_guard<std::mutex> lock(connection_attempts_mutex);

        // 移除超过1秒的旧记录
        auto now = std::chrono::steady_clock::now();
        while (!connection_attempts.empty() && now - connection_attempts.front() > std::chrono::seconds(1))
            connection_attempts.pop_front();

        // 检查连接速率
        if (connection_attempts.size() >= MAX_CONNECTIONS_PER_SECOND)
            return false; // 超过每秒最大连接尝试次数，拒绝连接

        // 检查当前活跃连接数
        if (current_connections.load() >= MAX_CONNECTIONS)
            return false; // 超过最大连接数，拒绝连接

        // 接受新连接
        connection_attempts.push_back(now);
        current_connections.fetch_add(1); // 增加当前连接数
        return true;
    }

    // 连接关闭时调用
    void connection_closed()
    {
        current_connections.fetch_sub(1); // 减少当前连接数
    }

    // 客户端线程处理函数
    static void handle_client(int conn_fd, void *this_)
    {
        while (true)
        {
            // 6.接收数据
            char buf[1024];
            bzero(buf, sizeof(buf)); // 清空缓冲区
            // ssize_t recv_len = read(conn_fd, buf, sizeof(buf) - 1);
            ssize_t recv_len = recv(conn_fd, buf, sizeof(buf) - 1, 0);
            if (recv_len == -1)
            {
                log_msg(ERROR, "recv error: " + std::string(strerror(errno)));
                close(conn_fd);
                return;
            }
            else if (recv_len == 0) // 客户端关闭连接
            {
                log_msg(INFO, "connection closed by peer");
                close(conn_fd); // 防止文件描述符泄漏
                return;
            }
            buf[recv_len] = '\0';

            log_msg(INFO, "recv data: " + std::string(buf)); // 接收数据成功

            // 数据加工：将接收到的数据转换为大写
            for (int i = 0; i < recv_len; ++i)
            {
                buf[i] = toupper(buf[i]);
            }

            // 7.发送数据
            // ssize_t send_len = write(conn_fd, buf, strlen(buf));
            ssize_t send_len = send(conn_fd, buf, strlen(buf), 0);
            if (send_len == -1)
            {
                log_msg(ERROR, "send error: " + std::string(strerror(errno)));
                close(conn_fd);
                return;
            }
        }

        // 7.关闭连接
        close(conn_fd);

        // 8.减少当前连接数
        ((TcpServer *)this_)->connection_closed();
    }

    // 接受客户端连接并返回连接的文件描述符
    int accept_connection()
    {
        log_msg(INFO, "waiting for new connection...");
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr)); // 清空地址结构体
        socklen_t client_addr_len = sizeof(client_addr);

        // accept是阻塞函数，直到有客户端连接才返回
        int conn_fd = accept(listen_fd_, (struct sockaddr *)&client_addr, &client_addr_len); // 返回连接套接字
        if (conn_fd == -1)
        {
            log_msg(ERROR, "accept error: " + std::string(strerror(errno)));
            return -1; // 出错时返回-1
        }
        log_msg(INFO, "accept a new connection, conn_fd = " + std::to_string(conn_fd)); // 接受连接成功
        return conn_fd;                                                                 // 返回连接的文件描述符
    }
};