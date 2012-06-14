

all: master slave

master:
	c++ -ggdb -I ~/pvm3/include/ -o matcher_master matcher_master.cpp ~/pvm3/lib/LINUX/libpvm3.a
slave:
	c++ -ggdb -I ~/pvm3/include/ -o matcher_slave matcher_slave.cpp ~/pvm3/lib/LINUX/libpvm3.a
slave-debug:
	c++ -DDEBUG=1 -ggdb -I ~/pvm3/include/ -o matcher_slave matcher_slave.cpp ~/pvm3/lib/LINUX/libpvm3.a

install:
	mv matcher_master ~/pvm3/bin/LINUX
	mv matcher_slave ~/pvm3/bin/LINUX