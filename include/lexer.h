#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_WORD,
    TOK_PIPE,
    TOK_REDIR_IN,
    TOK_REDIR_OUT,
    TOK_REDIR_APPEND,
    TOK_BACKGROUND,
    TOK_END
} token_type_t;

typedef struct token {
    token_type_t  type;
    char         *value;
    struct token *next;
} token_t;

token_t *lexer_tokenize(const char *line);
void     lexer_free(token_t *tokens);

#endif
