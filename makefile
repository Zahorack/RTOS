all: client server

client: xsemaphore.c xspace.c xprocess.c xsocket.c xclient.c
	gcc -o client xsemaphore.c xspace.c xprocess.c xsocket.c xclient.c -lpthread
 
server: xsemaphore.c xspace.c xprocess.c xsocket.c xserver.c
	gcc -o server xsemaphore.c xspace.c xprocess.c xsocket.c xserver.c -lpthread
 
clean:
	rm -f client server

.DEFAULT_GOAL := all
