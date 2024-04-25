#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <memory>
#include "udp_client.hpp"

// udp_server的使用说明
static void Usage(const std::string &proc)
{
    std::cout << "\t\t\n使用方法:\n\t" << proc << " <服务器IP> <服务器端口>\n"
              << std::endl;
}

int main(int argc, char *argv[])
{
    // 检查参数数量
    if (argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }

    // 获取服务器IP和端口
    std::string server_ip = argv[1];
    int server_port = std::stoi(argv[2]);

    // 创建UdpClient对象
    std::unique_ptr<UdpClient> client(new UdpClient(server_ip, server_port));

    client->init();  // 初始化客户端
    client->start(); // 启动客户端

    return 0;
}