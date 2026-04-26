#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "lexer.h"

pipeline_t *parser_parse(token_t *tokens);
void        parser_free(pipeline_t *pipeline);

#endif
