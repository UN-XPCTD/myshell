/*
 * FILE: jobs.h
 * Purpose:Background job table interface
 */

#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#include "types.h"


//Add a new background job to the job table
void jobs_add(pid_t pid, const char *cmdline);

//clean up finished jobs
void jobs_reap(void);

//print all active background jobs
void jobs_list(void);

#endif
