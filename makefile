all: client server

client: xpacket.c xsemaphore.c xspace.c xspace.h xprocess.c xsocket.c xclient.c xtimer.c
	gcc -o client xpacket.c xsemaphore.c xspace.c xprocess.c xsocket.c xclient.c xtimer.c -lpthread -lrt -lncurses
 
server: xpacket.c xsemaphore.c xspace.c xspace.h xprocess.c xsocket.c xserver.c xtimer.c
	gcc -o server xpacket.c xsemaphore.c xspace.c xprocess.c xsocket.c xserver.c xtimer.c -lpthread -lrt -lncurses
 
clean:
	rm -f client server
run:
	./client 15 16 25 49 6 7 40 31

.DEFAULT_GOAL := all
