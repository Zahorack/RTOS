#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "xprocess.h"


processArgs_t createProcess(void (*childFunction)(void *chArg), void (*parentFunction)(void  *pArg), void *chArg, void *pArg){

	processArgs_t args;
	int id = fork();
	args.pid = getpid();
	args.ppid = getppid();

	//child precess
	if(id == 0) {
		printf("Child process!\n");
		childFunction(&chArg);
	}
	//parrent preocess
	else if(id > 0){
		printf("Parent precess!\n");
		parentFunction(&pArg);
	}
	else {
		perror("fork() failed!\n");
		exit(EXIT_FAILURE);
	}

	return args;
}
