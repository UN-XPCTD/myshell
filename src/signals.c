#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include "signals.h"
#include "jobs.h"

static void sigchld_handler(int sig) {
    (void)sig;
    jobs_reap();
}

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
