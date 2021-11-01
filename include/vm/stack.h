#pragma once
#include "value.h"

typedef struct {
    Value values[512];
    Value* stack_pointer;
} Stack;

Stack* stack_init();
void stack_push(Stack* stack, Value value);
Value stack_pop(Stack* stack);
Value stack_peek(const Stack* stack, int offset);

void stack_dump(const Stack* stack);


