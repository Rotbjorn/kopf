#include "vm/op.h"

const char* opcode_to_string(Opcode op) {
#define __OPCODE(opcode) \
    case opcode:\
        return #opcode;
    switch(op) {
ENUMERATE_OPCODES(__OPCODE)
    }
    return "Unknown";
#undef __OPCODE
}
