#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class UdpClient
{
private:
    std::string server_ip_; // Server IP
    uint16_t server_port_;  // Server port
    int sockfd;             // Socket file descriptor
    struct sockaddr_in server_addr;

public:
    UdpClient(const std::string &ip, uint16_t port)
        : server_ip_(ip), server_port_(port), sockfd(-1)
    {
        // std::cout << "UdpClient constructor" << std::endl;
    }

    ~UdpClient()
    {
        // std::cout << "UdpClient destructor" << std::endl;
        close(sockfd);
    }

    void init()
    {
        // Create a socket
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            std::cerr << "Error creating socket\n";
            exit(1);
        }

        // Set the server address
        bzero(&server_addr, sizeof(server_addr)); // Clear the server address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port_);
        server_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());
    }

    void start()
    {
        // Send data to the server
        std::string message;
        while (true)
        {
            std::cout << "Enter message to send (or 'exit' to quit): ";
            std::getline(std::cin, message); // Get the message from the user
            if (message == "exit")
                break;

            if (sendto(sockfd, message.c_str(), message.length(), 0,
                       (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            {
                // Error sending message
                std::cerr << "Error sending message\n";
                break;
            }

            // Receive data from the server
            char buf[1024];
            bzero(buf, sizeof(buf));
            if (recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL) < 0)
            {
                // Error receiving data
                std::cerr << "Error receiving data\n";
                break;
            }
            // Print the received data
            // std::cout << "Received data: " << buf << std::endl;
            std::cout << buf << std::endl;

        }
    }

};