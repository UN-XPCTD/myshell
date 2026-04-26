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

    if (n == 1) {
        if (builtins_exec(pipeline->head))
            return;
    }

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
            signal(SIGINT,  SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < n - 1)
                dup2(pipes[i][1], STDOUT_FILENO);

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

    for (int i = 0; i < n - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    if (!pipeline->background) {
        for (int i = 0; i < n; i++)
            waitpid(pids[i], NULL, 0);
    } else {
        jobs_add(pids[n - 1], build_cmdline(pipeline));
    }
}
