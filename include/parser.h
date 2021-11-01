#pragma once
#include "pch.h"
#include "token.h"
#include "ast.h"

typedef struct {
    Token* token;

    uint8_t bytecode[512];
    size_t index;
} Parser;


Parser* parser_init(Token* token);
AST* parser_parse(Parser* parser);
