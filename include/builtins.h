/**
 * @file builtins.h
 * @brief Interface for built-in shell commands.
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "types.h"

/**
 * @brief Check if a command is a built-in and execute it if so.
 *
 * Built-ins are executed directly in the shell process without forking.
 * Recognized built-ins: cd, exit, help, jobs.
 *
 * @param cmd The command to check and possibly execute.
 * @return 1 if the command was a built-in and was handled, 0 otherwise.
 */
int builtins_exec(cmd_t *cmd);

#endif
