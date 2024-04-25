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
#include <vector>
#include <queue>
#include <algorithm>
#include <thread>
#include <mutex>
#include <mysql/mysql.h>
#include <condition_variable>

struct MessageInfo
{
    std::string message;           // 消息内容
    struct sockaddr_in senderAddr; // 发送者地址
};

class UdpServer
{
public:
    // 构造函数
    UdpServer(uint16_t port, const std::string &ip = "0.0.0.0")
        : port_(port), ip_(ip), sockfd_(-1)
    {
        conn = mysql_init(nullptr); // 初始化 MySQL 连接
        if (!mysql_real_connect(conn, "127.0.0.1", "root", "Lxw010629.", "Linux", 0, NULL, 0))
        {
            std::cerr << "数据库连接失败: " << mysql_error(conn) << std::endl;
            exit(1); // 或处理错误
        }
    }

    // 析构函数
    ~UdpServer()
    {
        close(sockfd_);    // 关闭套接字
        mysql_close(conn); // 关闭 MySQL 连接
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
        start_sending(); // 启动发送线程

        char buf[1024];                 // 缓冲区
        struct sockaddr_in client_addr; // 客户端地址
        socklen_t len = sizeof(client_addr);

        // 3.接收数据
        while (true)
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

            // 处理客户端上线
            if (!isClientOnline(client_addr))
            {
                std::string echo_msg;
                if (strcmp(buf, "online") == 0)
                {
                    addOnlineClient(client_addr);
                    std::string client_ip = inet_ntoa(client_addr.sin_addr);
                    echo_msg = "欢迎 " + client_ip + ":" + std::to_string(ntohs(client_addr.sin_port)) + " 上线";
                    if (sendto(sockfd_, echo_msg.c_str(), echo_msg.length(), 0,
                               (struct sockaddr *)&client_addr, len) < 0)
                        std::cerr << "sendto 错误" << std::endl;
                    continue; // 不向其他客户端转发"online"命令
                }
                else
                {
                    echo_msg = "请先发送'online'命令上线";
                    if (sendto(sockfd_, echo_msg.c_str(), echo_msg.length(), 0,
                               (struct sockaddr *)&client_addr, len) < 0)
                        std::cerr << "sendto 错误" << std::endl;
                    continue;
                }
            }

            // 处理"offline"命令
            if (strcmp(buf, "offline") == 0)
            {
                std::string echo_msg = "下线成功";
                if (sendto(sockfd_, echo_msg.c_str(), echo_msg.length(), 0,
                           (struct sockaddr *)&client_addr, len) < 0)
                    std::cerr << "sendto 错误" << std::endl;
                removeClient(client_addr);
                continue;
            }

            std::string client_ip = inet_ntoa(client_addr.sin_addr);
            uint16_t client_port = ntohs(client_addr.sin_port);
            // 添加发送者信息到消息中
            std::string sender_info = client_ip + ":" + std::to_string(client_port);
            std::string message_with_sender_info = sender_info + " 发送: " + buf;

            // 打印客户端信息
            std::cout << "客户端 " << inet_ntoa(client_addr.sin_addr) << ":"
                      << ntohs(client_addr.sin_port) << " 发送消息: " << buf << std::endl;

            // 将消息添加到消息队列
            add_message_to_queue(message_with_sender_info, client_addr);
        }
    }

private:
    uint16_t port_;                                  // 端口号
    std::string ip_;                                 // IP地址
    int sockfd_;                                     // 套接字文件描述符
    MYSQL *conn;                                     // MySQL连接
    std::vector<struct sockaddr_in> online_clients_; // 在线客户端地址结构
    std::queue<MessageInfo> message_queue_;          // 存储MessageInfo
    std::mutex mtx_queue_;                           // 消息队列互斥锁
    std::mutex mtx_online_clients_;                  // 在线客户端互斥锁
    std::condition_variable cond_var_;               // 消息队列条件变量

    // 添加在线客户端
    void addOnlineClient(const struct sockaddr_in &client_addr)
    {
        auto it = std::find_if(online_clients_.begin(), online_clients_.end(), [&client_addr](const struct sockaddr_in &addr)
                               { return compareSockAddr(addr, client_addr); });
        mtx_online_clients_.lock();
        if (it == online_clients_.end())
            online_clients_.push_back(client_addr);
        mtx_online_clients_.unlock();

        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        uint16_t client_port = ntohs(client_addr.sin_port);

        std::string query = "INSERT INTO clients (ip, port) VALUES ('" +
                            client_ip + "', '" + std::to_string(client_port) + "')";
        if (mysql_query(conn, query.c_str()))
            std::cerr << "数据库插入失败: " << mysql_error(conn) << std::endl;

        std::string query_connect = "INSERT INTO client_connections (ip, port) VALUES ('" +
                                    client_ip + "', '" + std::to_string(client_port) + "')";
        if (mysql_query(conn, query_connect.c_str()))
            std::cerr << "数据库插入失败: " << mysql_error(conn) << std::endl;
    }

    // 判断客户端是否在线
    bool isClientOnline(const struct sockaddr_in &client_addr)
    {
        return std::find_if(online_clients_.begin(), online_clients_.end(), [&client_addr](const struct sockaddr_in &addr)
                            { return compareSockAddr(addr, client_addr); }) != online_clients_.end();
    }

    // 移除客户端地址
    void removeClient(const struct sockaddr_in &client_addr)
    {
        online_clients_.erase(std::remove_if(online_clients_.begin(), online_clients_.end(), [&client_addr](const struct sockaddr_in &addr)
                                             { return compareSockAddr(addr, client_addr); }),
                              online_clients_.end());

        // 将客户端IP地址转换为字符串
        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        // 将客户端端口号转换为字符串
        uint16_t client_port = ntohs(client_addr.sin_port);

        // 构造SQL DELETE语句
        std::string query = "DELETE FROM clients WHERE ip = '" + client_ip + "' AND port = " + std::to_string(client_port);

        // 执行SQL语句
        if (mysql_query(conn, query.c_str()))
        {
            std::cerr << "数据库删除失败: " << mysql_error(conn) << std::endl;
        }
    }

    // 比较两个 sockaddr_in 结构体是否相等
    static bool compareSockAddr(const struct sockaddr_in &a, const struct sockaddr_in &b)
    {
        return a.sin_addr.s_addr == b.sin_addr.s_addr && a.sin_port == b.sin_port;
    }

    void start_sending()
    {
        std::thread send_thread(&UdpServer::send_message, this);
        send_thread.detach(); // 分离线程
    }

    void send_message()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mtx_queue_);
            cond_var_.wait(lock, [this]
                           { return !message_queue_.empty(); });

            // 获取消息和发送者信息
            MessageInfo msgInfo = message_queue_.front();
            message_queue_.pop();
            lock.unlock(); // 解锁

            for (auto &addr : online_clients_)
            {
                // 检查是否为消息的发送者，如果是，则不发送消息给自己
                if (addr.sin_addr.s_addr == msgInfo.senderAddr.sin_addr.s_addr &&
                    addr.sin_port == msgInfo.senderAddr.sin_port)
                    continue; // 跳过发送步骤
                if (sendto(sockfd_, msgInfo.message.c_str(), msgInfo.message.length(), 0,
                           (struct sockaddr *)&addr, sizeof(addr)) < 0)
                    std::cerr << "sendto 错误" << std::endl;
            }
        }
    }

    void add_message_to_queue(const std::string &message, const struct sockaddr_in &senderAddr)
    {
        std::unique_lock<std::mutex> lock(mtx_queue_);
        message_queue_.push({message, senderAddr});
        lock.unlock();
        cond_var_.notify_one();
    }
};
