#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>

#define MAX_ARGS  128
#define MAX_JOBS  64
#define MAX_LINE  1024

typedef struct cmd {
    char       *argv[MAX_ARGS];
    char       *infile;
    char       *outfile;
    int         append;
    struct cmd *next;
} cmd_t;

typedef struct {
    cmd_t *head;
    int    background;
} pipeline_t;

typedef struct {
    int   active;
    pid_t pid;
    char  cmdline[MAX_LINE];
} job_t;

#endif
