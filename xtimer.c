#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "xtimer.h"


static timer_t createTimer(int signal)
{
	timer_t timer;
	struct sigevent kam;

	kam.sigev_notify = SIGEV_SIGNAL;
  	kam.sigev_signo = signal;
  	timer_create(CLOCK_REALTIME, &kam, &timer);

	return timer;
}

static void runTimer(timer_t tim, int seconds, int nano)
{
  struct itimerspec timIT;
  timIT.it_value.tv_sec = seconds;
  timIT.it_value.tv_nsec = nano;
  timIT.it_interval.tv_sec = seconds;
  timIT.it_interval.tv_nsec = nano;
  timer_settime(tim,CLOCK_REALTIME,&timIT,NULL);
}

void initTimer(void (*IRQ_handler)(), int sig, int seconds, int nanoseconds)
{
	if(signal(sig, IRQ_handler) == SIG_ERR) {
		printf("Cannot map signal \n");
	}
	timer_t timer = createTimer(sig);
	runTimer(timer, seconds, nanoseconds);

}
