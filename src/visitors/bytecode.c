#include "visitors/bytecode.h"
#include "vm/op.h"
#include "debug/timer.h"

static void visitor_visit(BytecodeVisitor *bc, AST *node);
static void visit_compound(BytecodeVisitor *bc, AST *node);
static void visit_integer(BytecodeVisitor *bc, AST *node);
static void visit_decimal(BytecodeVisitor *bc, AST *node);
static void visit_string(BytecodeVisitor *bc, AST *node);
static void visit_bin_op(BytecodeVisitor *bc, AST *node);
static void visit_unary_op(BytecodeVisitor *bc, AST *node);

void bytecode_parse_ast(BytecodeVisitor *bc, AST *root) {
    TIMER_START();
    visitor_visit(bc, root);
    bytecode_push(bc, OP_EXIT);
    TIMER_END();
}

void visitor_visit(BytecodeVisitor *bc, AST *node) {
    //printf("Visiting: %s\n", asttype_to_string(node->type));
    switch (node->type) {
        case AST_NO_OP:
            break;
        case AST_COMPOUND:
            visit_compound(bc, node);
            break;
        case AST_INTEGER:
            visit_integer(bc, node);
            break;
        case AST_DECIMAL:
            visit_decimal(bc, node);
            break;
        case AST_STRING:
            visit_string(bc, node);
            break;
        case AST_BIN_OP:
            visit_bin_op(bc, node);
            break;
        case AST_UNARY_OP:
            visit_unary_op(bc, node);
            break;
        default:
            printf("TODO (visitor_visit): %s\n", asttype_to_string(node->type));
            break;
    }
}

void bytecode_push(BytecodeVisitor *bc, uint8_t value) {
    bc->bytecode[bc->bytecode_size++] = value;
}
size_t bytecode_constant(BytecodeVisitor *bc, Value value) {
    bc->constants[bc->constants_size] = value;
    return bc->constants_size++;
}

static void visit_compound(BytecodeVisitor *bc, AST *node) {
    for (int i = 0; i < node->as.compound.size; i++) {
        AST *current_node = node->as.compound.nodes[i];
        visitor_visit(bc, current_node);
    }
}
static void visit_integer(BytecodeVisitor *bc, AST *node) {
    size_t index = bytecode_constant(bc, integer_value(node->as.integer.value));
    bytecode_push(bc, OP_CONST);
    bytecode_push(bc, index);
}
static void visit_decimal(BytecodeVisitor *bc, AST *node) {
}
static void visit_string(BytecodeVisitor *bc, AST *node) {
}
static void visit_bin_op(BytecodeVisitor *bc, AST *node) {
    visitor_visit(bc, node->as.bin_op.left);
    visitor_visit(bc, node->as.bin_op.right);
    switch (node->as.bin_op.op) {
        case TOKEN_PLUS:
            bytecode_push(bc, OP_ADD);
            break;
        case TOKEN_MINUS:
            bytecode_push(bc, OP_SUB);
            break;
        case TOKEN_ASTERISK:
            bytecode_push(bc, OP_MUL);
            break;
        case TOKEN_SLASH:
            bytecode_push(bc, OP_DIV);
            break;
        default:
            printf("visit_bin_op weird infix\n");
    }
}
static void visit_unary_op(BytecodeVisitor *bc, AST *node) {
    visitor_visit(bc, node->as.unary_op.operand);
    bytecode_push(bc, OP_NEGATE);
}
