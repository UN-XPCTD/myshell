# MyShell — Simple Unix Shell

COSC 4348 Systems Programming Project
Team: Mark Rodriguez, Russell Saenz, Tristan Rayo

## Build

    make

## Run

Interactive mode:

    ./mysh

Batch mode:

    ./mysh script.sh

## Built-in Commands

| Command      | Description                                      |
|---|---|
| cd [dir]     | Change directory. Defaults to $HOME if no arg.  |
| help         | Print usage summary.                             |
| jobs         | List active background jobs.                     |
| exit [code]  | Exit the shell with optional exit code.          |

## Operators

| Operator | Example             | Description                          |
|---|---|---|
| >        | ls > out.txt        | Redirect stdout to file (overwrite)  |
| >>       | echo hi >> out.txt  | Redirect stdout to file (append)     |
| <        | cat < in.txt        | Redirect stdin from file             |
| |        | ls | grep src       | Pipe stdout to stdin of next command |
| &        | sleep 10 &          | Run command in background            |

## Signal Handling

| Signal  | Key    | Behavior                                      |
|---|---|---|
| SIGINT  | Ctrl+C | Terminates foreground child; shell continues  |
| SIGTSTP | Ctrl+Z | Suspends foreground child; shell continues    |
| SIGCHLD | —      | Automatically reaps finished background jobs  |

## Architecture

    src/
      main.c      — REPL loop, interactive/batch mode detection
      lexer.c     — Tokenizer: splits input into token linked list
      parser.c    — Parser: builds pipeline_t/cmd_t structures
      executor.c  — Executor: fork/execvp, pipes, I/O redirection
      builtins.c  — Built-in commands: cd, exit, help, jobs
      signals.c   — Signal handlers: SIGINT, SIGTSTP, SIGCHLD
      jobs.c      — Background job table tracking

    include/
      types.h     — Shared structs: cmd_t, pipeline_t, job_t
      lexer.h     — token_t, token_type_t, lexer API
      parser.h    — Parser API
      executor.h  — Executor API
      builtins.h  — Builtins API
      signals.h   — Signals API
      jobs.h      — Jobs API

## Testing

    bash tests/run_all.sh

Tests cover: basic commands, I/O redirection, piping, built-ins, and batch mode.
Results: 18/18 passing.
