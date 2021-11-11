#include "vm/vm.h"
#include "debug/timer.h"
#include "pch.h"
#include "vm/op.h"
#include "skrivarn.h"

static void execute_instruction(VM *vm);
static uint8_t fetch(VM *vm);

void vm_execute(VM *vm) {
    vm->ip = vm->bytecode;
    vm->stack.stack_pointer = vm->stack.values;
    while (true) {
        uint8_t current_instruction = *vm->ip;
        if (current_instruction == OP_EXIT)
            break;
        execute_instruction(vm);
        // stack_dump(&vm->stack);
    }
}

static void execute_instruction(VM *vm) {
    Opcode instruction = fetch(vm);
    skrivarn_logf("%s", opcode_to_string(instruction));
    switch (instruction) {
        case OP_CONST: {
            size_t index = fetch(vm);
            stack_push(&vm->stack, vm->constants[index]);
            break;
        }
        case OP_INT_ADD: {
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);
            stack_push(&vm->stack, integer_value(first.as.integer + second.as.integer));
            break;
        }
        case OP_INT_SUB: {
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);

            stack_push(&vm->stack, integer_value(first.as.integer - second.as.integer));
            break;
        }
        case OP_INT_MUL: {
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);

            stack_push(&vm->stack, integer_value(first.as.integer * second.as.integer));
            break;
        }
        case OP_INT_DIV: {
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);
            stack_push(&vm->stack, integer_value(first.as.integer / second.as.integer));
            break;
        }
        case OP_DEC_ADD: {
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);
            stack_push(&vm->stack, decimal_value(first.as.decimal + second.as.decimal));
            break;
        }
        case OP_DEC_SUB: {
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);

            stack_push(&vm->stack, decimal_value(first.as.decimal - second.as.decimal));
            break;
        }
        case OP_DEC_MUL: {
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);

            stack_push(&vm->stack, decimal_value(first.as.decimal * second.as.decimal));
            break;
        }
        case OP_DEC_DIV: {
            Value first = stack_pop(&vm->stack);
            Value second = stack_pop(&vm->stack);
            stack_push(&vm->stack, decimal_value(first.as.decimal / second.as.decimal));
            break;
        }
        case OP_NEGATE: {
            Value value = stack_pop(&vm->stack);
            value.as.integer = -value.as.integer;
            stack_push(&vm->stack, value);
        }
    }
}

static uint8_t fetch(VM *vm) {
    uint8_t val = *vm->ip;
    vm->ip++;
    return val;
}
