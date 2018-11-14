#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
   pid_t my_pid, parent_pid;	
   my_pid = getpid();    parent_pid = getppid();
   printf("\n Moje pid je: %d\n\n", my_pid);
   printf("Pid mojho rodica je: %d\n\n", parent_pid);	
   
   printf("Vykonavam execl a spustam prikaz ps:\n");		
   execl("/bin/ps", "ps", NULL);

   printf("\n Tento printf sa nikdy nespusti\n \n ");
   exit(1);
}

