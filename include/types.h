/**
 * @file types.h
 * @brief Shared data structures used across all MyShell modules.
 */

#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>

#define MAX_ARGS  128
#define MAX_JOBS  64
#define MAX_LINE  1024

/**
 * @brief Represents a single command in a pipeline.
 *
 * Commands are stored as a linked list. Each node holds the argument
 * vector, optional redirection filenames, and a pointer to the next
 * command in the pipeline.
 */
typedef struct cmd {
    char       *argv[MAX_ARGS]; /**< NULL-terminated argument vector */
    char       *infile;         /**< Input redirection filename, or NULL */
    char       *outfile;        /**< Output redirection filename, or NULL */
    int         append;         /**< 1 if output redirection is append mode */
    struct cmd *next;           /**< Next command in pipeline, or NULL */
} cmd_t;

/**
 * @brief Represents a complete pipeline of one or more commands.
 */
typedef struct {
    cmd_t *head;       /**< First command in the pipeline */
    int    background; /**< 1 if pipeline should run in background */
} pipeline_t;

/**
 * @brief Represents a background job in the job table.
 */
typedef struct {
    int   active;          /**< 1 if job is still running */
    pid_t pid;             /**< Process ID of the job */
    char  cmdline[MAX_LINE]; /**< Command string for display */
} job_t;

#endif

