#include "poll_server.hpp"

std::string transaction(const std::string &request)
{
    return "response: " + request;
}

int main()
{
    PollServer server(transaction);

    server.init();
    server.start();

    return 0;
}