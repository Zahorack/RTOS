#ifndef XPROCESS_H_
#define XPROCESS_H_

typedef  struct {
        pid_t pid;
        pid_t ppid;

} processArgs_t;

processArgs_t newProcess(void (*childFunction)(void), void (*parentFunction)(void));

#endif
