#include "tcp_client.hpp"
#include <memory>

// tcp_server的使用说明
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

    std::string ip = argv[1];      // 服务器IP
    uint16_t port = atoi(argv[2]); // 服务器端口

    std::shared_ptr<TcpClient> client = std::make_shared<TcpClient>(ip, port); // 创建客户端
    client->init();                                                            // 初始化客户端
    client->start();                                                           // 启动客户端

    return 0;
}