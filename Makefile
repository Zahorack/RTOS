all: xsemaphore.c xspace.c xprocess.c xsocket.c xserver.c
	gcc -o server xsemaphore.c xspace.c xprocess.c xsocket.c xserver.c -lpthread
