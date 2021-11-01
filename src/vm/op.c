#include "vm/op.h"

const char* opcode_to_string(Opcode op) {
#define __OPCODE(i, opcode) \
    case opcode:\
        return #opcode;
    switch(op) {
ENUMERATE_OPCODES(__OPCODE)
    }
#undef __OPCODE
}
