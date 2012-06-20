CC=gcc
CFLAGS=-ggdb -Wall -D_FILE_OFFSET_BITS=64
LDFLAGS=

all: score freqlist hash_dumper
score: score.c hash.c pstring.c pstring_queue.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o score score.c hash.c pstring.c pstring_queue.c

freqlist: freqlist.c hash.c pstring.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o freqlist freqlist.c hash.c pstring.c

google_frequency_list_dmalloc: google_frequency_list.cpp hash.hpp pstring.hpp
	g++ $(CFLAGS) $(LDFLAGS) -DDMALLOC -DDMALLOC_FUNC_CHECK -ldmalloc -o google_frequency_list google_frequency_list.cpp hash.hpp pstring.hpp
	
google_frequency_list: google_frequency_list.cpp hash.hpp pstring.hpp
	g++ $(CFLAGS) $(LDFLAGS) -o google_frequency_list google_frequency_list.cpp hash.hpp pstring.hpp

hash_dumper: hash_dumper.c hash.c pstring.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o hash_dumper hash_dumper.c hash.c pstring.c

matcher: matcher.c hash.c pstring.c int_stack.c pstring_queue.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o matcher matcher.c pstring.c int_stack.c pstring_queue.c