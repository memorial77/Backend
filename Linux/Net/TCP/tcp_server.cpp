#include "tcp_server.hpp"

int main()
{
    TcpServer server(8888);
    server.start();
    return 0;
}