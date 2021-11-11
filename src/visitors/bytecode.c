#include "visitors/bytecode.h"
#include "vm/op.h"
#include "debug/timer.h"
#include <rotbjorn/skrivarn.h>

static AST* visitor_visit(BytecodeVisitor *bc, AST *node);
static AST* visit_compound(BytecodeVisitor *bc, AST *node);
static AST* visit_integer(BytecodeVisitor *bc, AST *node);
static AST* visit_decimal(BytecodeVisitor *bc, AST *node);
static AST* visit_string(BytecodeVisitor *bc, AST *node);
static AST* visit_bin_op(BytecodeVisitor *bc, AST *node);
static AST* visit_unary_op(BytecodeVisitor *bc, AST *node);

void bytecode_parse_ast(BytecodeVisitor *bc, AST *root) {
    visitor_visit(bc, root);
    bytecode_push(bc, OP_EXIT);
}

static AST* visitor_visit(BytecodeVisitor *bc, AST *node) {
    printf("Visiting: %s\n", asttype_to_string(node->type));
    switch (node->type) {
        case AST_NO_OP:
            break;
        case AST_COMPOUND:
            return visit_compound(bc, node);
        case AST_INTEGER:
            return visit_integer(bc, node);
        case AST_DECIMAL:
            return visit_decimal(bc, node);
        case AST_STRING:
            return visit_string(bc, node);
        case AST_BIN_OP:
            return visit_bin_op(bc, node);
        case AST_UNARY_OP:
            return visit_unary_op(bc, node);
        default:
            printf("TODO (visitor_visit): %s\n", asttype_to_string(node->type));
            break;
    }
    return NULL;
}

void bytecode_push(BytecodeVisitor *bc, uint8_t value) {
    bc->bytecode[bc->bytecode_size++] = value;
}
size_t bytecode_constant(BytecodeVisitor *bc, Value value) {
    bc->constants[bc->constants_size] = value;
    return bc->constants_size++;
}

static AST* visit_compound(BytecodeVisitor *bc, AST *node) {
    for (int i = 0; i < node->as.compound.size; i++) {
        AST *current_node = node->as.compound.nodes[i];
        visitor_visit(bc, current_node);
    }
    return NULL;
}
static AST* visit_integer(BytecodeVisitor *bc, AST *node) {
    size_t index = bytecode_constant(bc, integer_value(node->as.integer.value));
    bytecode_push(bc, OP_CONST);
    bytecode_push(bc, index);
    return node;
}
static AST* visit_decimal(BytecodeVisitor *bc, AST *node) {
    size_t index = bytecode_constant(bc, decimal_value(node->as.decimal.value));
    bytecode_push(bc, OP_CONST);
    bytecode_push(bc, index);
    return node;
}
static AST* visit_string(BytecodeVisitor *bc, AST *node) {
    skrivarn_error("visit_string not yet implemented!");
    return NULL;
}
static AST* visit_bin_op(BytecodeVisitor *bc, AST *node) {
    AST* left = visitor_visit(bc, node->as.bin_op.left);
    AST* right = visitor_visit(bc, node->as.bin_op.right);
    TokenType op = node->as.bin_op.op;
    
    if(left->type != right->type) {
        skrivarn_warnf("%s is not %s\n", asttype_to_string(left->type), asttype_to_string(right->type));
        skrivarn_error("LEFT AND RIGHT IS NOT SAME TYPE (visit_bin_op)\n");
    }

    switch(left->type) {
        case AST_INTEGER:
            switch(op) {
                case TOKEN_PLUS:
                    bytecode_push(bc, OP_INT_ADD);
                    break;
                case TOKEN_MINUS:
                    bytecode_push(bc, OP_INT_SUB);
                    break;
                case TOKEN_ASTERISK:
                    bytecode_push(bc, OP_INT_MUL);
                    break;
                case TOKEN_SLASH:
                    bytecode_push(bc, OP_INT_DIV);
                    break;
                default:
                    skrivarn_error("Can't add those two types... ");
            }
            break;
        case AST_DECIMAL:
            switch(op) {
                case TOKEN_PLUS:
                    bytecode_push(bc, OP_DEC_ADD);
                    break;
                case TOKEN_MINUS:
                    bytecode_push(bc, OP_DEC_SUB);
                    break;
                case TOKEN_ASTERISK:
                    bytecode_push(bc, OP_DEC_MUL);
                    break;
                case TOKEN_SLASH:
                    bytecode_push(bc, OP_DEC_DIV);
                    break;
                default:
                    skrivarn_error("Can't add those two types... ");
            }
            break;
        default:
            skrivarn_error("No binop on this AST bro... (visit_bin_op)");
    }
    printf("DONE HERE\n");
    return left;
}
static AST* visit_unary_op(BytecodeVisitor *bc, AST *node) {
    AST* operand_node = visitor_visit(bc, node->as.unary_op.operand);
    bytecode_push(bc, OP_NEGATE);
    return operand_node;
}
