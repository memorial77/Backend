# Compiler and compiler flags
CC = g++
# -Wall: show all warnings, -g: enable debugging, -std=c++11: use C++11 standard
CFLAGS = -g -std=c++11 -pthread

all: a.out

a.out: UnitTests.cpp ThreadCache.cpp CentralCache.cpp PageCache.cpp
	$(CC) $(CFLAGS) UnitTests.cpp ThreadCache.cpp CentralCache.cpp PageCache.cpp -o a.out

clean:
	rm -f a.out