#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "types.h"
#include "executor.h"
#include "builtins.h"

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

static int count_cmds(pipeline_t *pipeline) {
    int n = 0;
    for (cmd_t *c = pipeline->head; c; c = c->next) n++;
    return n;
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

    /* build n-1 pipes */
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

            /* wire up pipe input from previous command */
            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);

            /* wire up pipe output to next command */
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
        printf("[%d] background\n", pids[n - 1]);
    }
}
