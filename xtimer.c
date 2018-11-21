#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "xtimer.h"

void timerIRQ(int sig) {

	//if(sig == SIGUSR1)
		printf("Prijaty signal z casovaca!\n");
}

/*USAGE*/
/*
int main(void)
{
	 initTimer(timerIRQ, SIGUSR1, 3);
 	 while(1)
 	 {
   		 sleep(1);
   		 printf("Dalsia sekunda\n");
 	 }
}

*/
void initTimer(void (*IRQ_handler)(), int sig, int time)
{
	if(signal(sig, IRQ_handler) == SIG_ERR) {
		printf("Cannot map signal \n");
	}
	timer_t timer = createTimer(sig);
	runTimer(timer, time);

}


timer_t createTimer(int signal)
{
	timer_t timer;
	struct sigevent kam;

	kam.sigev_notify = SIGEV_SIGNAL;
  	kam.sigev_signo = signal;
  	timer_create(CLOCK_REALTIME, &kam, &timer);

	return timer;
}

void runTimer(timer_t tim, int seconds)
{
  struct itimerspec timIT;
  timIT.it_value.tv_sec = seconds;
  timIT.it_value.tv_nsec=0;
  timIT.it_interval.tv_sec = seconds;
  timIT.it_interval.tv_nsec=0;
  timer_settime(tim,CLOCK_REALTIME,&timIT,NULL);
}

