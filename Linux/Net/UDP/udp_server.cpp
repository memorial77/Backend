#include "udp_server.hpp"
#include <stdlib.h>
#include <memory>

// Usage for udp_server
static void Usage(const std::string &proc)
{
    std::cout << "\t\t\nUsage:\n\t" << proc << "server_ip server_port\n"
              << std::endl;
}

// ./udp_server ip port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    std::string server_ip = argv[1];      // IP address
    uint16_t server_port = atoi(argv[2]); // Port number

    // Create a UdpServer object
    std::unique_ptr<UdpServer> server(new UdpServer(server_port, server_ip));

    server->init(); // Initialize the server
    server->start(); // Start the server
    
    return 0;
}