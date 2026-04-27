/**
 * @file parser.c
 * @brief Parser that converts a token list into a pipeline_t structure.
 *
 * Walks the token list produced by the lexer and builds a linked list
 * of cmd_t nodes. Each pipe token starts a new cmd_t. Redirection
 * tokens consume the following word as a filename. The background
 * token sets the pipeline background flag.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "lexer.h"
#include "parser.h"

/**
 * @brief Allocate and zero-initialize a new cmd_t.
 * @return Pointer to the new command node.
 */
static cmd_t *make_cmd(void) {
    cmd_t *c = calloc(1, sizeof(cmd_t));
    return c;
}

pipeline_t *parser_parse(token_t *tokens) {
    if (!tokens) return NULL;

    pipeline_t *pipeline = calloc(1, sizeof(pipeline_t));
    cmd_t *cmd = make_cmd();
    pipeline->head = cmd;
    int argc = 0;

    for (token_t *t = tokens; t != NULL; t = t->next) {
        switch (t->type) {

        case TOK_WORD:
            if (argc < MAX_ARGS - 1) {
                cmd->argv[argc++] = strdup(t->value);
                cmd->argv[argc]   = NULL;
            }
            break;

        case TOK_REDIR_IN:
            if (t->next && t->next->type == TOK_WORD) {
                cmd->infile = strdup(t->next->value);
                t = t->next;
            }
            break;

        case TOK_REDIR_OUT:
            if (t->next && t->next->type == TOK_WORD) {
                cmd->outfile = strdup(t->next->value);
                cmd->append  = 0;
                t = t->next;
            }
            break;

        case TOK_REDIR_APPEND:
            if (t->next && t->next->type == TOK_WORD) {
                cmd->outfile = strdup(t->next->value);
                cmd->append  = 1;
                t = t->next;
            }
            break;

        case TOK_PIPE:
            cmd->next = make_cmd();
            cmd       = cmd->next;
            argc      = 0;
            break;

        case TOK_BACKGROUND:
            pipeline->background = 1;
            break;

        default:
            break;
        }
    }

    return pipeline;
}

void parser_free(pipeline_t *pipeline) {
    if (!pipeline) return;
    cmd_t *cmd = pipeline->head;
    while (cmd) {
        for (int i = 0; cmd->argv[i]; i++)
            free(cmd->argv[i]);
        free(cmd->infile);
        free(cmd->outfile);
        cmd_t *next = cmd->next;
        free(cmd);
        cmd = next;
    }
    free(pipeline);
}
