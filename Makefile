CC=g++
CFLAGS=-Wall -std=c++11
LDFLAGS=-lpthread
DEBUG_FLAGS=-DDEBUG  -ggdb  # -DDEBUG2
all: matcher
debug: matcher_debug

matcher: matcher.cpp
	$(CC) -o matcher $(CFLAGS) $(LDFLAGS) matcher.cpp

matcher_debug: matcher.cpp
	$(CC) -o matcher $(CFLAGS) $(LDFLAGS) $(DEBUG_FLAGS) matcher.cpp
