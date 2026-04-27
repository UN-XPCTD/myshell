/**
 * @file signals.h
 * @brief Signal handler initialization interface.
 */

#ifndef SIGNALS_H
#define SIGNALS_H

/**
 * @brief Install signal handlers for the shell process.
 *
 * SIGINT and SIGTSTP are ignored in the shell so Ctrl+C and Ctrl+Z
 * only affect foreground children. SIGCHLD is handled to asynchronously
 * reap background children via jobs_reap().
 */
void signals_init(void);

#endif
