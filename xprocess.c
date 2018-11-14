#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "xprocess.h"


processArgs_t newProcess(void (*childFunction)(void), void (*parentFunction)(void)){

	processArgs_t args;
	args.pid = fork();
	args.ppid = getppid();

	//child precess
	if(args.pid == 0) {
		printf("Child process!\n");
		childFunction();
	}
	//parrent preocess
	else if(args.pid > 0){
		printf("Parent precess!\n");
		parentFunction();
	}
	else {
		perror("fork() failed!\n");
		exit(EXIT_FAILURE);
	}

	return args;
}
