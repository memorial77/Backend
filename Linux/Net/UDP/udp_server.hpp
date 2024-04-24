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

class UdpServer
{
public:
    UdpServer(uint16_t port, const std::string &ip = "0.0.0.0")
        : port_(port), ip_(ip), sockfd_(-1)
    {
        // std::cout << "UdpServer constructor" << std::endl;
    }

    ~UdpServer()
    {
        // std::cout << "UdpServer destructor" << std::endl;
        close(sockfd_); // Close the socket
    }

    // Initialize the server
    void init()
    {
        // 1.Create a socket
        // AF_INET: IPv4 or AF_INET6: IPv6
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ < 0)
        {
            std::cerr << "socket error" << std::endl; // Print error message
            exit(1);
        }

        // 2.Bind the socket
        struct sockaddr_in server_addr;                       // Server address
        bzero(&server_addr, sizeof(server_addr));             // Clear the server address
        server_addr.sin_family = AF_INET;                     // IPv4
        server_addr.sin_port = htons(port_);                  // Port number
        server_addr.sin_addr.s_addr = inet_addr(ip_.c_str()); // IP address
        // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);       // Accept any IP address

        if (bind(sockfd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            std::cerr << "bind error" << std::endl;
            exit(2);
        }

        // Print the server information
        std::cout << "Server is running on " << ip_ << ":" << port_ << std::endl;
    }

    // Start the server
    void start()
    {
        char buf[1024];                 // Buffer
        struct sockaddr_in client_addr; // Client address
        socklen_t len = sizeof(client_addr);

        // 3.Receive data
        while (1)
        {
            memset(buf, 0, sizeof(buf)); // Clear the buffer
            // Receive data from the client, leaving space for the null terminator
            ssize_t recv_size = recvfrom(sockfd_, buf, sizeof(buf) - 1, 0,
                                         (struct sockaddr *)&client_addr, &len);
            if (recv_size < 0)
            {
                std::cerr << "recvfrom error" << std::endl;
                // Continue to the next iteration of the loop
                continue;
            }

            // Manually add a null terminator to the received data
            buf[recv_size] = '\0';

            // Now buf is a null-terminated string
            // Print the client information and the received message
            std::string client_ip = inet_ntoa(client_addr.sin_addr);
            uint16_t client_port = ntohs(client_addr.sin_port);
            // std::cout << "Client [" << client_ip << ":"
            //           << client_port << "] says: " << buf << std::endl;

            FILE* fp = popen(buf, "r");
            if (fp == NULL)
            {
                std::cerr << "popen error" << std::endl;
                break;
            }

            // Read the output of the command
            std::string output;
            char temp[1024];
            // Read the output of the command line by line
            while (fgets(temp, sizeof(temp), fp) != NULL)
            {
                output += temp;
            }
            pclose(fp);

            // If the client sends "exit", the server will exit
            if (strcmp(buf, "exit") == 0)
            {
                break;
            }

            // Send a response to the client
            // std::string response = "I got your message";
            // if (sendto(sockfd_, response.c_str(), response.length(), 0,
            //            (struct sockaddr *)&client_addr, len) < 0)
            // {
            //     std::cerr << "sendto error" << std::endl;
            //     break;
            // }
            if(sendto(sockfd_, output.c_str(), output.length(), 0,
                      (struct sockaddr *)&client_addr, len) < 0)
            {
                std::cerr << "sendto error" << std::endl;
                break;
            }
        }
    }

private:
    uint16_t port_;  //  Port number
    std::string ip_; // IP address
    int sockfd_;     // Socket file descriptor
};
