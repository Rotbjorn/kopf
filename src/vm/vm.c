#include "vm/vm.h"
#include "pch.h"
#include "vm/op.h"
#include "debug/timer.h"

static void execute_instruction(VM *vm);
static uint8_t fetch(VM *vm);

void vm_execute(VM *vm) {
    TIMER_START();
    vm->ip = vm->bytecode;
    vm->stack.stack_pointer = vm->stack.values;
    while (true) {
        uint8_t current_instruction = *vm->ip;
        if(current_instruction == OP_EXIT)
            break;
        execute_instruction(vm);
        //stack_dump(&vm->stack);
    }
    TIMER_END();
}

static void execute_instruction(VM *vm) {
    switch (fetch(vm)) {
        case OP_CONST: {
            printf("OP_CONST");
            size_t index = fetch(vm);
            printf(" %ld\n", index);
            stack_push(&vm->stack, vm->constants[index]);
            break;
        }
        case OP_ADD: {
            printf("OP_ADD\n");
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);

            if(first.type == second.type) {
                // TODO: fix hardcoded integer cast
                stack_push(&vm->stack, integer_value(first.as.integer + second.as.integer));
            }
            break;
        }
        case OP_SUB: {
            printf("OP_SUB\n");
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);

            if(first.type == second.type) {
                // TODO: fix hardcoded integer cast
                stack_push(&vm->stack, integer_value(first.as.integer - second.as.integer));
            }
            break;
        }
        case OP_MUL: {
            printf("OP_MUL\n");
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);

            if(first.type == second.type) {
                // TODO: fix hardcoded integer cast
                stack_push(&vm->stack, integer_value(first.as.integer * second.as.integer));
            }
            break;
        }
        case OP_DIV: {
            printf("OP_DIV\n");
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);

            if(first.type == second.type) {
                // TODO: fix hardcoded integer cast
                stack_push(&vm->stack, integer_value(first.as.integer / second.as.integer));
            }
            break;
        }
        case OP_NEGATE: {
            printf("OP_NEGATE\n");
            Value value = stack_pop(&vm->stack);
            value.as.integer = - value.as.integer;
            stack_push(&vm->stack, value);
        }
    }
}

static uint8_t fetch(VM *vm) {
    uint8_t val = *vm->ip;
    vm->ip++;
    return val;
}
