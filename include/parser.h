/**
 * @file parser.h
 * @brief Parser interface for building pipeline structures from tokens.
 */

#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "lexer.h"

/**
 * @brief Parse a token list into a pipeline structure.
 *
 * Builds a pipeline_t containing one cmd_t per pipeline stage.
 * Redirection filenames and the background flag are extracted here.
 *
 * @param tokens Head of the token list produced by the lexer.
 * @return Pointer to a heap-allocated pipeline_t, or NULL on empty input.
 */
pipeline_t *parser_parse(token_t *tokens);

/**
 * @brief Free all memory associated with a pipeline structure.
 * @param pipeline Pipeline to free.
 */
void parser_free(pipeline_t *pipeline);

#endif
