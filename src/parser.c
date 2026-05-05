/*
 * FILE: parser.c
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

//allocate and init a new cmd_t
//return pointer to new command node
static cmd_t *make_cmd(void) {
    cmd_t *c = calloc(1, sizeof(cmd_t));
    return c;
}

//parsing the linked list
//itterate through tokens and collect command arguments
pipeline_t *parser_parse(token_t *tokens) {

    //BASE CASE
    //if no tokens -> return NULL
    if (!tokens) return NULL;

    pipeline_t *pipeline = calloc(1, sizeof(pipeline_t));
    cmd_t *cmd = make_cmd();
    pipeline->head = cmd;
    int argc = 0;

    //go through every token
    for (token_t *current_token = tokens; current_token != NULL; current_token = current_token->next) {
        switch (current_token->type) {

            //command or arg
            case TOK_WORD:
                if (argc < MAX_ARGS - 1) {
                    cmd->argv[argc++] = strdup(current_token->value);
                    cmd->argv[argc]   = NULL;
                }
                break;

            //input redirect
            case TOK_REDIR_IN:
                if (current_token->next && current_token->next->type == TOK_WORD) {
                    //next token is assumed to be infile
                    cmd->infile = strdup(current_token->next->value);
                    current_token = current_token->next;
                }
                break;

            //output redirect
            case TOK_REDIR_OUT:
                if (current_token->next && current_token->next->type == TOK_WORD) {
                    //next token is assumed to be outfile
                    cmd->outfile = strdup(current_token->next->value);
                    cmd->append  = 0;
                    current_token = current_token->next;
                }
                break;

            //append redirection
            case TOK_REDIR_APPEND:
                if (current_token->next && current_token->next->type == TOK_WORD) {
                    //next token is assumed to be outfile
                    cmd->outfile = strdup(current_token->next->value);
                    cmd->append  = 1;
                    current_token = current_token->next;
                }
                break;

            //pipe opperator
            case TOK_PIPE:
                //make aditional cmd
                cmd->next = make_cmd();
                cmd       = cmd->next;
                argc      = 0;
                break;

                //background operator
            case TOK_BACKGROUND:
                pipeline->background = 1;
                break;

            default:
                break;
            }
    }

    return pipeline;
}

//free up memory
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
