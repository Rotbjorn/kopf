#include "datatypes.h"
#include "debug/timer.h"
#include "visitors/bytecode.h"
#include "vm/op.h"
#include <rotbjorn/skrivarn.h>

/*
 * TODO: Remove KopfDataType with CompiledExpr or something similar.
 * Type check by checking the latest pushed Instruction (struct)
 */


size_t lookup(SymbolTable *table, const char *key);
void symboltable_dump(SymbolTable *table);

static KopfDataType visitor_visit(BytecodeVisitor *bc, AST *node);

static KopfDataType visit_compound(BytecodeVisitor *bc, AST *node);
static KopfDataType visit_block(BytecodeVisitor *bc, AST *node);

static KopfDataType visit_variable_declaration(BytecodeVisitor *bc, AST *node);

static KopfDataType visit_bin_op(BytecodeVisitor *bc, AST *node);
static KopfDataType visit_unary_op(BytecodeVisitor *bc, AST *node);

static KopfDataType visit_integer(BytecodeVisitor *bc, AST *node);
static KopfDataType visit_decimal(BytecodeVisitor *bc, AST *node);
static KopfDataType visit_string(BytecodeVisitor *bc, AST *node);

static KopfDataType visit_variable(BytecodeVisitor *bc, AST *node);

void bytecode_parse_ast(BytecodeVisitor *bc, AST *root) {
    bc->table = symboltable_init();
    visitor_visit(bc, root);
    bytecode_push(bc, OP_EXIT);
}

SymbolTable *symboltable_init() {
    SymbolTable *symbol_table = malloc(sizeof(struct SymbolTable));

    symbol_table->parent = NULL;
    symbol_table->table = table_init();

    return symbol_table;
}

size_t lookup(SymbolTable *table, const char *key) {
    size_t *index;

    if ((index = table_get(table->table, key)) == NULL) {
        if (table->parent == NULL) {
            skrivarn_errorf("DIDN'T FIND %s", key);
            return -1;
        }
        return lookup(table->parent, key);
    }

    size_t returned_index = *index;
    return returned_index;
}

void symboltable_dump(SymbolTable *table) {
    for (int i = 0; i < KOPF_TABLE_SIZE; i++) {
        Entry *entry = table->table->entries[i];

        if (entry == NULL) {
            continue;
        }

        skrivarn_infof("Symbol(key='%s', type='%d')", entry->key, *((KopfDataType*)entry->value));
    }
}

static KopfDataType visitor_visit(BytecodeVisitor *bc, AST *node) {
    static_assert(AST_SIZE == 11, "Exhaustive stuff");
    skrivarn_infof("Visiting: %s", asttype_to_string(node->type));
    switch (node->type) {
        case AST_NO_OP:
            break;
        case AST_COMPOUND:
            return visit_compound(bc, node);
        case AST_BLOCK:
            return visit_block(bc, node);
        case AST_VARIABLE_DECLARATION:
            return visit_variable_declaration(bc, node);
        case AST_BIN_OP:
            return visit_bin_op(bc, node);
        case AST_UNARY_OP:
            return visit_unary_op(bc, node);
        case AST_INTEGER:
            return visit_integer(bc, node);
        case AST_DECIMAL:
            return visit_decimal(bc, node);
        case AST_STRING:
            return visit_string(bc, node);
        case AST_VARIABLE:
            return visit_variable(bc, node);
        default:
            printf("TODO (visitor_visit): %s\n", asttype_to_string(node->type));
            break;
    }
    return -1;
}

void bytecode_push(BytecodeVisitor *bc, uint8_t value) {
    bc->bytecode[bc->bytecode_size++] = value;
}

void bytecode_op(BytecodeVisitor *bc, Opcode op) {
    skrivarn_infof("Pushing: %s", opcode_to_string(op));
    bytecode_push(bc, op);
}

void bytecode_ops(BytecodeVisitor *bc, Opcode op, uint8_t operand) {
    skrivarn_infof("Pushing: %s %d", opcode_to_string(op), operand);
    bytecode_push(bc, op);
    bytecode_push(bc, operand);
}

size_t bytecode_constant(BytecodeVisitor *bc, Value value) {
    bc->constants[bc->constants_size] = value;
    return bc->constants_size++;
}

static KopfDataType visit_compound(BytecodeVisitor *bc, AST *node) {
    for (int i = 0; i < node->as.compound.size; i++) {
        AST *current_node = node->as.compound.nodes[i];
        visitor_visit(bc, current_node);
    }
    return -1;
}

static KopfDataType visit_block(BytecodeVisitor *bc, AST *node) {
    SymbolTable *scope = symboltable_init();

    scope->parent = bc->table;
    bc->table = scope;

    for (int i = 0; i < node->as.compound.size; i++) {
        AST *current_node = node->as.compound.nodes[i];
        visitor_visit(bc, current_node);
    }

    // TODO: Free the scope
    bc->table = scope->parent;
    
    skrivarn_error("HERE????");

    return -1;
}

static KopfDataType visit_variable_declaration(BytecodeVisitor *bc, AST *node) {
    KopfDataType rhs = visitor_visit(bc, node->as.var_decl.value);

    if (rhs != node->as.var_decl.type) {
        skrivarn_errorf("Can't assign %d to %d", rhs, node->as.var_decl.type);
        exit(1);
    }

    const char *var_name = node->as.var_decl.name;

    size_t *index = malloc(sizeof(size_t));
    *index = bc->global_variables_count++;

    // TODO: before setting, make sure entry doesn't exist (variable already defined)
    table_set(bc->table->table, var_name, (void*)index);

    bytecode_ops(bc, OP_DEFINE_GLOBAL, *index);

    return node->as.var_decl.type;
}

static KopfDataType visit_integer(BytecodeVisitor *bc, AST *node) {
    size_t index = bytecode_constant(bc, integer_value(node->as.integer.value));
    bytecode_ops(bc, OP_CONST, index);
    return KOPF_DATA_TYPE_INT;
}
static KopfDataType visit_decimal(BytecodeVisitor *bc, AST *node) {
    size_t index = bytecode_constant(bc, decimal_value(node->as.decimal.value));
    bytecode_ops(bc, OP_CONST, index);
    return KOPF_DATA_TYPE_DECIMAL;
}
static KopfDataType visit_string(BytecodeVisitor *bc, AST *node) {
    skrivarn_error("visit_string not yet implemented!");
    return KOPF_DATA_TYPE_STRING;
}

static KopfDataType visit_variable(BytecodeVisitor *bc, AST *node) {
    const char *var_name = node->as.variable.name;

    //TODO: if table parent is NULL, it is in global scope
    size_t index = lookup(bc->table, var_name);

    skrivarn_errorf("Index here: %d", index);

    if (index == -1) {
        skrivarn_errorf("Couldn't find variable of name: %s", var_name);
        exit(0);
    }

    bytecode_ops(bc, OP_GET_GLOBAL, index);

    return KOPF_DATA_TYPE_DECIMAL;
}

static KopfDataType visit_bin_op(BytecodeVisitor *bc, AST *node) {
    KopfDataType right = visitor_visit(bc, node->as.bin_op.right);
    KopfDataType left = visitor_visit(bc, node->as.bin_op.left);
    TokenType op = node->as.bin_op.op;

    skrivarn_errorf("LHS: %d, RHS: %d", left, right);

    if (left != right) {
        skrivarn_warnf("%d is not %d\n", left, right);
        skrivarn_error("LEFT AND RIGHT IS NOT SAME TYPE (visit_bin_op)\n");
        exit(1);
    }

    switch (left) {
        case KOPF_DATA_TYPE_INT:
            switch (op) {
                case TOKEN_PLUS:
                    bytecode_op(bc, OP_INT_ADD);
                    break;
                case TOKEN_MINUS:
                    bytecode_op(bc, OP_INT_SUB);
                    break;
                case TOKEN_ASTERISK:
                    bytecode_op(bc, OP_INT_MUL);
                    break;
                case TOKEN_SLASH:
                    bytecode_op(bc, OP_INT_DIV);
                    break;
                default:
                    skrivarn_error("Can't add those two types... ");
            }
            break;
        case KOPF_DATA_TYPE_DECIMAL:
            switch (op) {
                case TOKEN_PLUS:
                    bytecode_op(bc, OP_DEC_ADD);
                    break;
                case TOKEN_MINUS:
                    bytecode_op(bc, OP_DEC_SUB);
                    break;
                case TOKEN_ASTERISK:
                    bytecode_op(bc, OP_DEC_MUL);
                    break;
                case TOKEN_SLASH:
                    bytecode_op(bc, OP_DEC_DIV);
                    break;
                default:
                    skrivarn_error("Can't add those two types... ");
            }
            break;
        default:
            skrivarn_error("No binop on this type bro... (visit_bin_op)");
    }

    return left;
}
static KopfDataType visit_unary_op(BytecodeVisitor *bc, AST *node) {
    KopfDataType operand_type = visitor_visit(bc, node->as.unary_op.operand);
    bytecode_op(bc, OP_NEGATE);
    return operand_type;
}
