# Compiler and compiler flags
CC = g++
# -Wall: show all warnings, -g: enable debugging, -std=c++11: use C++11 standard
CFLAGS = -Wall -g -std=c++11 

# Targets and prerequisites
all: udp_client udp_server

udp_client: udp_client.cpp udp_client.hpp
	$(CC) $(CFLAGS) udp_client.cpp -o udp_client

udp_server: udp_server.cpp udp_server.hpp
	$(CC) $(CFLAGS) udp_server.cpp -o udp_server

clean:
	rm -f udp_client udp_server