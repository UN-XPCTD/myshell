/**
 * @file jobs.h
 * @brief Background job table interface.
 */

#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#include "types.h"

/**
 * @brief Add a new background job to the job table.
 * @param pid     PID of the background process.
 * @param cmdline Command string to display in job listings.
 */
void jobs_add(pid_t pid, const char *cmdline);

/**
 * @brief Reap any finished background jobs and print completion notices.
 *
 * Uses waitpid with WNOHANG so it never blocks. Called from the
 * SIGCHLD handler.
 */
void jobs_reap(void);

/**
 * @brief Print all currently active background jobs.
 */
void jobs_list(void);

#endif
