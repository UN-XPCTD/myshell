/*
 * FILE: signals.c
 *
 * PURPOSE: SIGINT and SIGTSTP are ignored in the shell process so that Ctrl+C
 * and Ctrl+Z only affect the foreground child. Each child resets these
 * to SIG_DFL after fork. SIGCHLD is handled to asynchronously reap
 * background children without creating zombie processes.
 */

#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include "signals.h"
#include "jobs.h"


//sigchld_handler reaps finished child processes using jobs_reap()
static void sigchld_handler(int sig) {
    (void)sig;
    jobs_reap();
}


//initialize singals that are used later in the prorgam
void signals_init(void) {
    struct sigaction sa_ign  = {0};
    struct sigaction sa_chld = {0};

    sa_ign.sa_handler = SIG_IGN;
    sigemptyset(&sa_ign.sa_mask);

    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART;

    sigaction(SIGINT,  &sa_ign,  NULL);
    sigaction(SIGTSTP, &sa_ign,  NULL);
    sigaction(SIGCHLD, &sa_chld, NULL);
}
