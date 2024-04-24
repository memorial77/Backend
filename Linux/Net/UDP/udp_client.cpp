#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <memory>
#include "udp_client.hpp"

int main(int argc, char *argv[])
{
    // Check the number of arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <ServerIP> <ServerPort>\n";
        return 1;
    }

    // Get the server IP and port
    std::string server_ip = argv[1];
    int server_port = std::stoi(argv[2]);

    // Create a UdpClient object
    std::unique_ptr<UdpClient> client(new UdpClient(server_ip, server_port));

    client->init();  // Initialize the client
    client->start(); // Start the client

    return 0;
}