#include "udp_server.hpp"
#include <stdlib.h>
#include <memory>

// udp_server的使用说明
static void Usage(const std::string &proc)
{
    std::cout << "\t\t\n使用方法:\n\t" << proc << " <端口号>\n"
              << std::endl;
}

// 使用方式 ./udp_server ip port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1); // 如果参数不正确则显示使用方法后退出
    }

    uint16_t server_port = atoi(argv[1]); // 服务端口号

    // 创建UdpServer对象
    std::unique_ptr<UdpServer> server(new UdpServer(server_port));

    server->init(); // 初始化服务器
    server->start(); // 启动服务器
    
    return 0;
}