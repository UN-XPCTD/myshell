/*
 * FILE: parser.h
 * Puspose: parser interface for building pipeline structures from tokens
 */

#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "lexer.h"

//parsing the linked list
//itterate through tokens and collect command arguments
pipeline_t *parser_parse(token_t *tokens);

//free up memory used
void parser_free(pipeline_t *pipeline);

#endif
