#pragma once
#include "ast.h"
#include "pch.h"
#include "vm/value.h"
#include "vm/table.h"
#include "vm/op.h"


// TODO: Why in header????
typedef struct SymbolTable SymbolTable;

typedef struct SymbolTable {
    Table* table;
    SymbolTable *parent;
} SymbolTable;


typedef struct BytecodeVisitor {
    //TODO: Create Instruction struct with more info
    uint8_t bytecode[256];
    size_t bytecode_size;

    Value constants[64];
    size_t constants_size;

    size_t global_variables_count;

    SymbolTable *table;
} BytecodeVisitor;

void bytecode_parse_ast(BytecodeVisitor *, AST *root);
void bytecode_push(BytecodeVisitor *, uint8_t value);
void bytecode_op(BytecodeVisitor *, Opcode op);
size_t bytecode_constant(BytecodeVisitor *, Value value);

SymbolTable *symboltable_init();
void symboltable_free(SymbolTable **symboltable);
