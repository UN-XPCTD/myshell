/**
 * @file builtins.c
 * @brief Built-in command implementations for MyShell.
 *
 * Built-ins are executed directly in the shell process without forking.
 * builtins_exec() returns 1 if it handled the command, 0 if the command
 * should be passed to the executor as an external program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "types.h"
#include "builtins.h"
#include "jobs.h"

/**
 * @brief Change the current working directory.
 *
 * Uses chdir(2). Defaults to HOME if no argument is given.
 *
 * @param argv Argument vector where argv[1] is the target directory.
 * @return Always returns 1 (handled).
 */
static int builtin_cd(char **argv) {
    char *dir = argv[1];
    if (!dir)
        dir = getenv("HOME");
    if (chdir(dir) != 0)
        perror("cd");
    return 1;
}

/**
 * @brief Print a usage summary of available commands and operators.
 * @return Always returns 1 (handled).
 */
static int builtin_help(void) {
    printf("MyShell — Simple Unix Shell\n");
    printf("Built-in commands:\n");
    printf("  cd [dir]       Change directory (defaults to HOME)\n");
    printf("  help           Show this help message\n");
    printf("  jobs           List background jobs\n");
    printf("  exit [code]    Exit the shell\n");
    printf("Operators:\n");
    printf("  cmd > file     Redirect output to file\n");
    printf("  cmd >> file    Append output to file\n");
    printf("  cmd < file     Redirect input from file\n");
    printf("  cmd1 | cmd2    Pipe output of cmd1 to cmd2\n");
    printf("  cmd &          Run command in background\n");
    return 1;
}

int builtins_exec(cmd_t *cmd) {
    if (!cmd->argv[0])
        return 0;

    if (strcmp(cmd->argv[0], "cd") == 0)
        return builtin_cd(cmd->argv);

    if (strcmp(cmd->argv[0], "help") == 0)
        return builtin_help();

    if (strcmp(cmd->argv[0], "jobs") == 0) {
        jobs_list();
        return 1;
    }

    if (strcmp(cmd->argv[0], "exit") == 0) {
        int code = cmd->argv[1] ? atoi(cmd->argv[1]) : 0;
        exit(code);
    }

    return 0;
}
