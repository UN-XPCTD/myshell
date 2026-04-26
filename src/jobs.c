#include <stdio.h>
#include "types.h"
#include "jobs.h"

void jobs_add(pid_t pid, const char *cmdline) {
    (void)pid; (void)cmdline;
}

void jobs_reap(void) {}

void jobs_list(void) {}
