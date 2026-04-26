#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#include "types.h"

void jobs_add(pid_t pid, const char *cmdline);
void jobs_reap(void);
void jobs_list(void);

#endif
