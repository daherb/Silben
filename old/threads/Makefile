CC=g++
CFLAGS=-Wall -std=c++11
LDFLAGS=-lpthread
DEBUG_FLAGS=-DDEBUG -ggdb 
all: matcher
debug: matcher_debug

matcher: matcher.cpp matcher.hpp
	$(CC) -o matcher $(CFLAGS) $(LDFLAGS) matcher.cpp

matcher_debug: matcher.cpp matcher.hpp
	$(CC) -o matcher $(CFLAGS) $(LDFLAGS) $(DEBUG_FLAGS) matcher.cpp
