#pragma once
#include "pch.h"
#include "token.h"
#include "ast.h"

typedef struct {
    Token* token;

    size_t index;
} Parser;


Parser* parser_init(Token* token);
void parser_free(Parser **parser);

AST* parser_generate_ast(Parser* parser);
