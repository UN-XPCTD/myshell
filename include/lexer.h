/**
 * @file lexer.h
 * @brief Lexer interface for tokenizing shell input.
 */

#ifndef LEXER_H
#define LEXER_H

/**
 * @brief Token types recognized by the lexer.
 */
typedef enum {
    TOK_WORD,         /**< A command name or argument */
    TOK_PIPE,         /**< The pipe operator | */
    TOK_REDIR_IN,     /**< Input redirection < */
    TOK_REDIR_OUT,    /**< Output redirection > */
    TOK_REDIR_APPEND, /**< Append redirection >> */
    TOK_BACKGROUND,   /**< Background operator & */
    TOK_END           /**< End of token list */
} token_type_t;

/**
 * @brief A single token in the input stream.
 */
typedef struct token {
    token_type_t  type;  /**< Type of this token */
    char         *value; /**< String value for TOK_WORD, NULL otherwise */
    struct token *next;  /**< Next token in the list */
} token_t;

/**
 * @brief Tokenize a raw input line into a linked list of tokens.
 * @param line Null-terminated input string to tokenize.
 * @return Pointer to the head of the token list, or NULL if input is empty.
 */
token_t *lexer_tokenize(const char *line);

/**
 * @brief Free all memory associated with a token list.
 * @param tokens Head of the token list to free.
 */
void lexer_free(token_t *tokens);

#endif

