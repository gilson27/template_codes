/**
    \File timer.c
    \Overview Template for timer implementation in Linux
    \Author Gilson Varghese<gilsonvarghese7@gmail.com>
*/
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define TIMER_INTERVAL 10
#define CLOCK_INTERVAL_SEC 1
#define CLOCK_INTERVAL_NSEC 0
#define SLEEP_TIME 20 // in seconds
#define SIG SIGUSR1

timer_t timerid;

void timer_handler(int signal_val, siginfo_t *si, void *data) {
    printf("Inside Timer handler signal:[%d], timer_id:[%lx] original %lx\n", signal_val, *(long int *)si->si_value.sival_ptr, (long int)timerid);
    // signal(signal_val, SIG_IGN);
    if(si->si_value.sival_ptr != &timerid){
        printf("Stray signal\n");
    } else {
        printf("Caught signal %d from timer\n", signal_val);
    }
}

/**
    Create a timer which sends signal n milliseconds
    /params time_in_ms Timer interval in milliseconds
    /return Timer creation status
*/
int createTimer(int time_in_ms) {
    struct sigevent sev;
    
    struct sigaction sa;
    struct itimerspec its;
    sigset_t mask;

    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask); 
    sa.sa_flags = SA_SIGINFO;
    if(sigaction(SIG, &sa, NULL) == -1) {
        fprintf(stderr, "Signal could not be registsred: %s",strerror(errno));
        goto sigaction_failed;
    }

    /** Blocking Timer temporarily */
    // sigemptyset(&mask);
    // sigaddset(&mask, SIG);
    // if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
    //     fprintf(stderr, " Failed to mask the process %s\n", strerror(errno));
    //     goto sigprocmask_failed;
    // }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if(timer_create(CLOCK_REALTIME, &sev, &timerid)) {
        fprintf(stderr, "Failed to create timer: %s", strerror(errno));
        goto timer_create_failed;
    }
    fprintf(stdout, "TimerID=%lx[%lx]\n", (long int)timerid, *(long int*)sev.sigev_value.sival_ptr);
    its.it_interval.tv_sec = CLOCK_INTERVAL_SEC;
    its.it_interval.tv_nsec = CLOCK_INTERVAL_NSEC;
    its.it_value.tv_sec = CLOCK_INTERVAL_SEC;
    its.it_value.tv_nsec = CLOCK_INTERVAL_NSEC;

    if(timer_settime(timerid, 0, &its, NULL)) {
        fprintf(stderr, "Failed to set time %s\n", strerror(errno));
        goto set_time_failed;
    }

    // if(sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
    //     fprintf(stderr, " Failed to mask the process %s\n", strerror(errno));
    // }
    return 0;
set_time_failed:
timer_create_failed:
sigprocmask_failed:
sigaction_failed:
    return -1;
}

int main() {
    int status = 0;
    status = createTimer(TIMER_INTERVAL);
    fprintf(stdout, "Done create timer with rv = %d\n", status);
    fprintf(stdout, "Going to Sleep %d seconds...\n", (int)SLEEP_TIME);
    while(1) {
        sleep(20);
    }
    
    return 0;
}
