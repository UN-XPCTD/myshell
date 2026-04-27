/**
 * @file jobs.c
 * @brief Background job table for tracking and reaping background processes.
 *
 * Maintains a static array of job_t entries. Jobs are added when a
 * background command is launched and removed when the process exits.
 * jobs_reap() is called from the SIGCHLD handler to collect exit status
 * without blocking.
 */

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "types.h"
#include "jobs.h"

/** @brief Static job table shared across all job functions. */
static job_t job_table[MAX_JOBS];

void jobs_add(pid_t pid, const char *cmdline) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!job_table[i].active) {
            job_table[i].active = 1;
            job_table[i].pid    = pid;
            strncpy(job_table[i].cmdline, cmdline, MAX_LINE - 1);
            printf("[%d] %d\n", i + 1, pid);
            return;
        }
    }
    fprintf(stderr, "mysh: job table full\n");
}

void jobs_reap(void) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (job_table[i].active && job_table[i].pid == pid) {
                printf("[%d] done  %s\n", i + 1, job_table[i].cmdline);
                job_table[i].active = 0;
            }
        }
    }
}

void jobs_list(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_table[i].active)
            printf("[%d] running  %s\n", i + 1, job_table[i].cmdline);
    }
}
