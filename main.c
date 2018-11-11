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



//>>>>>>>>>>>>>>>>>  MAIN  <<<<<<<<<<<<<<<<<<<<<
int main(int argc, char *argv[]) {


	initSpace(argc,argv);

	initSemaphore();

	deinitSemaphore();

	return 0;
}
