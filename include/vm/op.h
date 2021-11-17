#pragma once

#define ENUMERATE_OPCODES(O) \
    O(OP_NOOP)               \
    O(OP_CONST)              \
    /* Arithmetic opcodes */ \
    O(OP_INT_ADD)            \
    O(OP_INT_SUB)            \
    O(OP_INT_MUL)            \
    O(OP_INT_DIV)            \
                             \
    O(OP_DEC_ADD)            \
    O(OP_DEC_SUB)            \
    O(OP_DEC_MUL)            \
    O(OP_DEC_DIV)            \
                             \
    O(OP_NEGATE)             \
    O(OP_DEFINE_GLOBAL)      \
    O(OP_GET_GLOBAL)         \
    O(OP_EXIT)               \
    O(OPCODES_SIZE)

#define __OPCODES(op) op,
typedef enum Opcode { ENUMERATE_OPCODES(__OPCODES) } Opcode;
#undef __OPCODES

const char *opcode_to_string(Opcode);
