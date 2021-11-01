#pragma once
#include "token.h"

typedef struct {
    const char* begin;
    const char* end;
} Lexer;


Lexer* lexer_init();
Token* lexer_parse(Lexer* lexer, const char* source);
