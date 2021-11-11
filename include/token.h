#pragma once
#include "pch.h"

#define ENUMERATE_TOKENS(O) \
    O(TOKEN_IDENTIFIER)     \
    O(TOKEN_STRING)         \
    O(TOKEN_INTEGER)        \
    O(TOKEN_DECIMAL)        \
                            \
    O(TOKEN_KW_IF)          \
    O(TOKEN_KW_ELSE)        \
    O(TOKEN_KW_TYPE)        \
                            \
    O(TOKEN_PLUS)           \
    O(TOKEN_MINUS)          \
    O(TOKEN_ASTERISK)       \
    O(TOKEN_SLASH)          \
                            \
    O(TOKEN_NOT)            \
    O(TOKEN_EQ)             \
    O(TOKEN_NOT_EQ)         \
    O(TOKEN_EQ_EQ)          \
                            \
    O(TOKEN_DOT)            \
    O(TOKEN_COMMA)          \
    O(TOKEN_COLON)          \
    O(TOKEN_SEMICOLON)      \
                            \
    O(TOKEN_LPAREN)         \
    O(TOKEN_RPAREN)         \
    O(TOKEN_LBRACE)         \
    O(TOKEN_RBRACE)         \
    O(END_OF_FILE)

#define __TOKENS(token) \
    token,
typedef enum {
    ENUMERATE_TOKENS(__TOKENS)
} TokenType;
#undef __TOKENS

typedef struct {
    TokenType type;
    const char *value;
    size_t length;
} Token;

const char *tokentype_to_string(TokenType type);
Token token_create(TokenType type, const char *value, size_t length);
char *token_value_copy(Token *token);

void token_print(Token *token);
