all: main.c xsemaphore.c xspace.c
	gcc -o program main.c xsemaphore.c xspace.c -lpthread
