#pragma once
#include "token.h"

typedef struct {
    const char* begin;
    const char* end;
} Lexer;


Lexer* lexer_init();
void lexer_free(Lexer **lexer);
Token* lexer_lex(Lexer* lexer, const char* source);
