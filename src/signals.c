/**
 * @file signals.c
 * @brief Signal handler setup for MyShell.
 *
 * SIGINT and SIGTSTP are ignored in the shell process so that Ctrl+C
 * and Ctrl+Z only affect the foreground child. Each child resets these
 * to SIG_DFL after fork. SIGCHLD is handled to asynchronously reap
 * background children without creating zombie processes.
 */

#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include "signals.h"
#include "jobs.h"

/**
 * @brief SIGCHLD handler — reaps finished background children.
 *
 * Delegates to jobs_reap() which uses WNOHANG so it never blocks.
 * Only background jobs tracked in the job table produce output.
 *
 * @param sig Signal number (unused).
 */
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
