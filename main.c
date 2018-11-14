/* FEI STU, predmet RTOS
 * Semestralne zadanie
 *
 * Autor: Oliver Holly
 * Date:  11.11.2018
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "xsemaphore.h"
#include "main.h"
#include "xspace.h"
#include "xprocess.h"
#include "xsocket.h"

//::Function declarations
static void childFcn();
static void parentFcn();
static void emptyFcn();


//>>>>>>>>>>>>>>>>>  MAIN  <<<<<<<<<<<<<<<<<<<<<
int main(int argc, char *argv[]){

	initSpace(argc,argv);
	initServerSocket();
	processArgs_t prc = newProcess(childFcn,emptyFcn);

	initSemaphore();

	deinitSemaphore();

	return 0;
}
//<<<<<<<<<<<<<<<<<  MAIN  >>>>>>>>>>>>>>>>>>>>


//::Function definitions
static void childFcn(){
	for(int i=0; i < 1; i++){
		printf("I am child process\n");
	}
}

static void parentFcn(){
	for(int i = 0; i < 1; i++){
		printf("I am parrent process\n");
	}
}

static void emptyFcn(){};
