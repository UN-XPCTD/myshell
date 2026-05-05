/*
 * FILE: executor.c
 *
 * PURPOSE: This file handles the execution of commands including single commands, multistage pipelines, and
 * input/output redirection and backgrounf processes.
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

// applies in/out redirection for a command in child process
//opens specific files
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

//counts how many commands are in a pipeline
static int count_cmds(pipeline_t *pipeline) {
    int numOfCmds = 0;
    for (cmd_t *c = pipeline->head; c; c = c->next) numOfCmds++;
    return numOfCmds;
}

//turns the pipeline ints a character string
static char *build_cmdline(pipeline_t *pipeline) {
    static char buf[MAX_LINE];
    buf[0] = '\0';
    for (cmd_t *cmd = pipeline->head; cmd; cmd = cmd->next) {
        for (int i = 0; cmd->argv[i]; i++) {
            strncat(buf, cmd->argv[i], MAX_LINE - strlen(buf) - 1);
            strncat(buf, " ",       MAX_LINE - strlen(buf) - 1);
        }
        if (cmd->next)
            strncat(buf, "| ", MAX_LINE - strlen(buf) - 1);
    }
    return buf;
}

//entry point of commands
//forks child process for every command in the pipeline and sets up pipes for redirection
void executor_run(pipeline_t *pipeline) {
    if (!pipeline || !pipeline->head || !pipeline->head->argv[0])
        return;

    int numOfCmds = count_cmds(pipeline);

    //single command, check builtins before forking
    if (numOfCmds == 1) {
        if (builtins_exec(pipeline->head))
            return;
    }

    // allocate n-1 pipes
    int pipes[numOfCmds][2];
    for (int i = 0; i < numOfCmds - 1; i++) {
        if (pipe(pipes[i]) < 0) { perror("pipe"); return; }
    }

    pid_t pids[numOfCmds];
    cmd_t *cmd = pipeline->head;

    for (int i = 0; i < numOfCmds; i++, cmd = cmd->next) {
        pids[i] = fork();
        if (pids[i] < 0) { perror("fork"); return; }

        if (pids[i] == 0) {
            //child: restore default signal behavior
            signal(SIGINT,  SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            // connect pipe input from previous command
            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);

            //connect pipe output to next command
            if (i < numOfCmds - 1)
                dup2(pipes[i][1], STDOUT_FILENO);

            // close all pipe ends in child
            for (int j = 0; j < numOfCmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            apply_redirection(cmd);
            execvp(cmd->argv[0], cmd->argv);
            perror(cmd->argv[0]);
            exit(1);
        }
    }

    //parent: close all pipe ends
    for (int i = 0; i < numOfCmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // wait for all children unless background
    if (!pipeline->background) {
        for (int i = 0; i < numOfCmds; i++)
            waitpid(pids[i], NULL, 0);
    } else {
        jobs_add(pids[numOfCmds - 1], build_cmdline(pipeline));
    }
}
