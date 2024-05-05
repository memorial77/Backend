#include "select_server.hpp"

std::string transaction(const std::string &request)
{
    return "response: " + request;
}

int main()
{
    SelectServer server(transaction);
    server.init();
    server.start();
    return 0;
}