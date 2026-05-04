/*
 * FILE: main.c
 *
 * Supports interactive mode (prompt displayed, reads from stdin) and
 * batch mode (reads commands from a file without a prompt).
 * Uses isatty() to detect whether stdin is a terminal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "types.h"
#include "lexer.h"
#include "parser.h"
#include "executor.h"
#include "signals.h"

//header of each line in shell
#define PROMPT "mysh> "

//uses a constant loop to read and execute
//recieves 1 for interactive, 0 for non
static void run_loop(FILE *input, int interactive) {
    char line[MAX_LINE];

    while (1) {
        if (interactive) {
            printf("%s", PROMPT);
            fflush(stdout);
        }

        if (!fgets(line, sizeof(line), input))
            break;

        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0')
            continue;

        token_t    *tokens   = lexer_tokenize(line);
        pipeline_t *pipeline = parser_parse(tokens);

        //if pipeline == NULL then no tokens were found
        if (pipeline)
            executor_run(pipeline);

        lexer_free(tokens);
        parser_free(pipeline);
    }
}

//With no arguments: runs interactively.
// With one argument: runs in batch mode reading from that file.
int main(int argc, char *argv[]) {
    signals_init();

    if (argc == 1) {
        int interactive = isatty(STDIN_FILENO);
        run_loop(stdin, interactive);
    } else {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            perror(argv[1]);
            return 1;
        }
        run_loop(f, 0);
        fclose(f);
    }

    return 0;
}
