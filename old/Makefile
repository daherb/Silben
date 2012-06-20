CC=g++
CFLAGS=-ggdb -Wall -D_FILE_OFFSET_BITS=64 -std=c++11
LDFLAGS=

all: score freqlist hash_dumper
score: score.cpp hash.hpp pstring.hpp queue.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o score score.cpp hash.hpp pstring.hpp queue.hpp

freqlist: freqlist.cpp hash.hpp pstring.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o freqlist freqlist.cpp hash.hpp pstring.hpp

google_frequency_list_dmalloc: google_frequency_list.cpp hash.hpp pstring.hpp
	g++ $(CFLAGS) $(LDFLAGS) -DDMALLOC -DDMALLOC_FUNC_CHECK -ldmalloc -o google_frequency_list google_frequency_list.cpp hash.hpp pstring.hpp

google_frequency_list: google_frequency_list.cpp hash.hpp pstring.hpp
	g++ $(CFLAGS) $(LDFLAGS) -o google_frequency_list google_frequency_list.cpp hash.hpp pstring.hpp

hash_dumper: hash_dumper.cpp hash.hpp pstring.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o hash_dumper hash_dumper.c hash.hpp pstring.hpp

matcher: matcher.cpp hash.hpp pstring.hpp int_stack.c queue.hpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o matcher matcher.cpp stack.hpp queue.hpp
