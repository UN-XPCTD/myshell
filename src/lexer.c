#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static token_t *make_token(token_type_t type, const char *value) {
    token_t *t = malloc(sizeof(token_t));
    t->type  = type;
    t->value = value ? strdup(value) : NULL;
    t->next  = NULL;
    return t;
}

token_t *lexer_tokenize(const char *line) {
    token_t *head = NULL;
    token_t *tail = NULL;
    const char *p = line;

    while (*p) {
        while (isspace((unsigned char)*p))
            p++;
        if (!*p) break;

        token_t *t = NULL;

        if (*p == '|') {
            t = make_token(TOK_PIPE, NULL);
            p++;
        } else if (*p == '&') {
            t = make_token(TOK_BACKGROUND, NULL);
            p++;
        } else if (*p == '<') {
            t = make_token(TOK_REDIR_IN, NULL);
            p++;
        } else if (*p == '>') {
            if (*(p + 1) == '>') {
                t = make_token(TOK_REDIR_APPEND, NULL);
                p += 2;
            } else {
                t = make_token(TOK_REDIR_OUT, NULL);
                p++;
            }
        } else {
            const char *start = p;
            while (*p && !isspace((unsigned char)*p) &&
                   *p != '|' && *p != '&' &&
                   *p != '<' && *p != '>')
                p++;
            char word[1024];
            int len = p - start;
            strncpy(word, start, len);
            word[len] = '\0';
            t = make_token(TOK_WORD, word);
        }

        if (!head) head = tail = t;
        else { tail->next = t; tail = t; }
    }

    return head;
}

void lexer_free(token_t *tokens) {
    while (tokens) {
        token_t *next = tokens->next;
        free(tokens->value);
        free(tokens);
        tokens = next;
    }
}
