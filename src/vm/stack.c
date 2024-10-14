#include "vm/stack.h"
#include "debug/color.h"

Stack* stack_init() {
    Stack* stack = malloc(sizeof(Stack));
    stack->stack_pointer = stack->values;
    return stack;
}

void stack_push(Stack* stack, Value value) {
    *stack->stack_pointer = value;
    stack->stack_pointer++;
}

Value stack_pop(Stack* stack) {
    return *--stack->stack_pointer;
}

Value stack_peek(const Stack* stack, int offset) {
    return *(stack->stack_pointer + offset);
}

void stack_dump(const Stack* stack) {
    for (int i = 0; i < (stack->stack_pointer - stack->values); i++) {
        printf(DARK_GRAY "[ " RESET);
        value_dump(&stack->values[i]);
        printf(DARK_GRAY " ]\n" RESET);
    }
}
