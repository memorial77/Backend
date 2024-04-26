#include "tcp_client.hpp"

int main()
{
    TcpClient client("127.0.0.1", 8888);
    client.start();
    return 0;
}