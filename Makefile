CC=g++
CFLAGS=-Wall -std=c++0x
LDFLAGS=-lpthread
DEBUG_FLAGS=-ggdb  -DSTATUS # -DDEBUG2 -DDEBUG
all: matcher_longfirst matcher_shortfirst matcher_dumb
debug: matcher_longfirst_debug matcher_shortfirst_debug matcher_dumb_debug

matcher_longfirst: matcher.cpp
	$(CC) -o matcher_longfirst $(CFLAGS) $(LDFLAGS) -DLONG_FIRST -DRESULT_FILE='(char *) "ergebnis_longfirst.txt"' matcher.cpp

matcher_longfirst_debug: matcher.cpp
	$(CC) -o matcher_longfirst_debug $(CFLAGS) $(LDFLAGS) $(DEBUG_FLAGS) -DLONG_FIRST -DRESULT_FILE='(char *) "ergebnis_longfirst.txt"' matcher.cpp

matcher_shortfirst: matcher.cpp
	$(CC) -o matcher_shortfirst $(CFLAGS) $(LDFLAGS) -DSHORT_FIRST -DRESULT_FILE='(char *) "ergebnis_shortfirst.txt"' matcher.cpp

matcher_shortfirst_debug: matcher.cpp
	$(CC) -o matcher_shortfirst_debug $(CFLAGS) $(LDFLAGS) $(DEBUG_FLAGS) -DSHORT_FIRST -DRESULT_FILE='(char *) "ergebnis_shortfirst.txt"' matcher.cpp

matcher_dumb: matcher.cpp
	$(CC) -o matcher_dumb $(CFLAGS) $(LDFLAGS) -DDUMB -DRESULT_FILE='(char *) "ergebnis_dumb.txt"' matcher.cpp

matcher_dumb_debug: matcher.cpp
	$(CC) -o matcher_dumb_debug $(CFLAGS) $(LDFLAGS) -DDUMB $(DEBUG_FLAGS) -DRESULT_FILE='(char *) "ergebnis_dumb.txt"' matcher.cpp