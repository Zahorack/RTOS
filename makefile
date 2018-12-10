all: client server

client: xpacket.c xmaze.c xsemaphore.c xspace.c xspace.h xprocess.c xsocket.c xclient.c xtimer.c xsharedmemory.c
	gcc -o client xmaze.c xpacket.c xsemaphore.c xspace.c xprocess.c xsocket.c xclient.c xtimer.c xsharedmemory.c -lpthread -lrt -lncurses
 
server: xpacket.c xsemaphore.c xmaze.c xspace.c xspace.h xprocess.c xsocket.c xserver.c xtimer.c xsharedmemory.c
	gcc -o server xpacket.c xmaze.c xsemaphore.c xspace.c xprocess.c xsocket.c xserver.c xtimer.c xsharedmemory.c -lpthread -lrt -lncurses
 
clean:
	rm -f client server
run:
	./client 25 25 15 9 6 7 16 20 20 15 18 3 6 24 60 25 55 9 48 16 40 25 35 8 30 14

.DEFAULT_GOAL := all
