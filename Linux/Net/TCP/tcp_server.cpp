#include "tcp_server.hpp"
#include <memory>

int main()
{
    std::shared_ptr<TcpServer> server(TcpServer::get_instance()); // 获取服务器对象
    server->init();                                               // 初始化服务器
    server->start();                                              // 启动服务器
    return 0;
}