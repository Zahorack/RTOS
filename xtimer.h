#ifndef XTIMER_H_
#define XTIMER_H_

//typedef void (*pFuncSocketArgs)(socketArgs_t *);


void initTimer(void (*fcn)(), int, int);
timer_t createTimer(int);
void runTimer(timer_t, int);

#endif
