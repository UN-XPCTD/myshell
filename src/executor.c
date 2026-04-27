/**
 * @file executor.c
 * @brief Pipeline executor using fork, execvp, pipe, and dup2.
 *
 * Handles single commands (with built-in check), multi-stage pipelines,
 * input/output redirection, and background execution. For a pipeline of
 * N commands, N-1 pipes are created and N child processes are forked.
 * Each child inherits only the file descriptors it needs; all others are
 * closed to prevent deadlocks from lingering write ends.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "types.h"
#include "executor.h"
#include "builtins.h"
#include "jobs.h"

/**
 * @brief Apply input/output redirection for a command in a child process.
 *
 * Opens the specified files and rewires STDIN/STDOUT via dup2.
 * Calls exit(1) on failure so the child does not continue.
 *
 * @param cmd Command whose redirection fields to apply.
 */
static void apply_redirection(cmd_t *cmd) {
    if (cmd->infile) {
        int fd = open(cmd->infile, O_RDONLY);
        if (fd < 0) { perror(cmd->infile); exit(1); }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (cmd->outfile) {
        int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
        int fd = open(cmd->outfile, flags, 0644);
        if (fd < 0) { perror(cmd->outfile); exit(1); }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

/**
 * @brief Count the number of commands in a pipeline.
 * @param pipeline Pipeline to count.
 * @return Number of cmd_t nodes in the pipeline.
 */
static int count_cmds(pipeline_t *pipeline) {
    int n = 0;
    for (cmd_t *c = pipeline->head; c; c = c->next) n++;
    return n;
}

/**
 * @brief Build a single command-line string from a pipeline for display.
 * @param pipeline Pipeline to stringify.
 * @return Pointer to a static buffer containing the command string.
 */
static char *build_cmdline(pipeline_t *pipeline) {
    static char buf[MAX_LINE];
    buf[0] = '\0';
    for (cmd_t *c = pipeline->head; c; c = c->next) {
        for (int i = 0; c->argv[i]; i++) {
            strncat(buf, c->argv[i], MAX_LINE - strlen(buf) - 1);
            strncat(buf, " ",       MAX_LINE - strlen(buf) - 1);
        }
        if (c->next)
            strncat(buf, "| ", MAX_LINE - strlen(buf) - 1);
    }
    return buf;
}

void executor_run(pipeline_t *pipeline) {
    if (!pipeline || !pipeline->head || !pipeline->head->argv[0])
        return;

    int n = count_cmds(pipeline);

    /* single command — check builtins before forking */
    if (n == 1) {
        if (builtins_exec(pipeline->head))
            return;
    }

    /* allocate n-1 pipes */
    int pipes[n][2];
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes[i]) < 0) { perror("pipe"); return; }
    }

    pid_t pids[n];
    cmd_t *cmd = pipeline->head;

    for (int i = 0; i < n; i++, cmd = cmd->next) {
        pids[i] = fork();
        if (pids[i] < 0) { perror("fork"); return; }

        if (pids[i] == 0) {
            /* child: restore default signal behavior */
            signal(SIGINT,  SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            /* connect pipe input from previous command */
            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);

            /* connect pipe output to next command */
            if (i < n - 1)
                dup2(pipes[i][1], STDOUT_FILENO);

            /* close all pipe ends in child */
            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            apply_redirection(cmd);
            execvp(cmd->argv[0], cmd->argv);
            perror(cmd->argv[0]);
            exit(1);
        }
    }

    /* parent: close all pipe ends */
    for (int i = 0; i < n - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /* wait for all children unless background */
    if (!pipeline->background) {
        for (int i = 0; i < n; i++)
            waitpid(pids[i], NULL, 0);
    } else {
        jobs_add(pids[n - 1], build_cmdline(pipeline));
    }
}
