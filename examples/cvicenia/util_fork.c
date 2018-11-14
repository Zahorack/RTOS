/* Fork - vydlicka

	zavolanie funkcie "fork()" vytvori presnu kopiu aktualneho procesu, novy preces (dieta) ktore bezi nezavistle od rodica 
	avsak vsetky vlastnosti pred zavolanim funkcie maju oba procesi rovnake

*/
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{

   printf("--beginning of program\n");


    int pocitadlo = 0;
    int counter = 0;
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process
	pocitadlo++;
	printf("pocitadlo: %d",pocitadlo);
	printf("\nchild\n");
	printf("cild pid: %d child ppd: %d\n",getpid(), getppid());

	pid_t pid2 = fork();
	if(pid2 == 0){
		//new child process
		printf("new child pid: %d new child ppid: %d\n", getpid(), getppid());
   	 }
	else if(pid2 > 0) {
		//new parrent process
//		sleep(2);
		printf("new parent pid: %d new  parent  ppid: %d\n", getpid(), getppid());

	}
}
    else if (pid > 0)
    {
//	sleep(5);
        // parent process
	pocitadlo++;
	printf("pocitadlo: %d", pocitadlo);
      printf("\nparent\n");
	printf("parent pid: %d  parent ppid: %d\n",getpid(), getppid());  
 }
    else
    {
        // fork failed
        printf("fork() failed!\n");
        return 1;
    }

    printf("--end of program--\n");

    return 0;
}

