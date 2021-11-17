#pragma once
#include "pch.h"
#include "stack.h"
#include "vm/table.h"


typedef Table GlobalSymbolTable;

typedef struct SymbolTable SymbolTable;

typedef struct {
    Stack stack;
    uint8_t* bytecode;
    uint8_t* ip;

    Value *globals;
    SymbolTable *symbols;

    Value constants[64];
} VM;


void vm_execute(VM *vm);
