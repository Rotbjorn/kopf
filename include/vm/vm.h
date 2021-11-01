#pragma once
#include "pch.h"
#include "stack.h"


typedef struct {
    Stack stack;
    uint8_t* bytecode;
    uint8_t* ip;

    Value constants[64];
} VM;

void vm_execute(VM *vm);
