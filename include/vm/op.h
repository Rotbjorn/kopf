#pragma once

#define ENUMERATE_OPCODES(O) \
    O(0x00, OP_NOOP)         \
    O(0x01, OP_CONST)        \
    /* Arithmetic opcodes */ \
    O(0x02, OP_ADD)          \
    O(0x03, OP_SUB)          \
    O(0x04, OP_MUL)          \
    O(0x05, OP_DIV)          \
                             \
    O(0x06, OP_NEGATE)       \
    O(0xFF, OP_EXIT)

#define __OPCODES(i, op) \
    op,
typedef enum Opcode {
    ENUMERATE_OPCODES(__OPCODES)
} Opcode;
#undef __OPCODES

const char *opcode_to_string(Opcode);
