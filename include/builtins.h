/*
 * FILE: builtins.h
 * Purpose: Interface for built-in shell commands.
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "types.h"

//executes the desired built-in command
int builtins_exec(cmd_t *cmd);

#endif
