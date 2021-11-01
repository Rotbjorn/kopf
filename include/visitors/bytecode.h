#pragma once
#include "ast.h"
#include "pch.h"
#include "vm/value.h"

typedef struct BytecodeVisitor {
    uint8_t bytecode[256];
    size_t bytecode_size;

    Value constants[64];
    size_t constants_size;
} BytecodeVisitor;

void bytecode_parse_ast(BytecodeVisitor *, AST *root);
void bytecode_push(BytecodeVisitor *, uint8_t value);
size_t bytecode_constant(BytecodeVisitor *, Value value);
