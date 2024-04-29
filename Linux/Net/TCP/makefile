# Compiler and compiler flags
CC = g++
# -Wall: show all warnings, -g: enable debugging, -std=c++11: use C++11 standard
CFLAGS = -Wall -g -std=c++11 -pthread

# # Linker flags
# LDFLAGS = -L/usr/lib64/mysql -lmysqlclient

# Targets and prerequisites
all: tcp_client tcp_server

tcp_client: tcp_client.cpp tcp_client.hpp
	$(CC) $(CFLAGS) tcp_client.cpp -o tcp_client

tcp_server: tcp_server.cpp tcp_server.hpp
	$(CC) $(CFLAGS) tcp_server.cpp -o tcp_server $(LDFLAGS)

clean:
	rm -f tcp_client tcp_server