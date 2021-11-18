#include "ast.h"

static int indent = 0;

const char *asttype_to_string(ASTType type) {
#define __AST(ast) \
    case ast:      \
        return #ast;

    switch (type) {
        ENUMERATE_AST(__AST)
    }
#undef __AST
}

static const char *binary_operator(TokenType type) {
    switch (type) {
        case TOKEN_PLUS:
            return "+";
            break;
        case TOKEN_MINUS:
            return "-";
            break;
        case TOKEN_ASTERISK:
            return "*";
            break;
        case TOKEN_SLASH:
            return "/";
            break;
        default:
            return "UNKNOWN binary_operator()";
            break;
    }
}

void print_indent() {
    printf("\n");
    for (int i = 0; i < indent; i++) {
        printf("    ");
    }
}

void ast_dump(AST *ast) {
    switch (ast->type) {
        case AST_NO_OP:
            printf("AST_NO_OP");
            break;

        case AST_COMPOUND:
            printf("AST_COMPOUND(");
            indent++;
            print_indent();
            printf("[");
            indent++;
            for (int i = 0; i < ast->as.compound.size; i++) {
                print_indent();
                ast_dump(ast->as.compound.nodes[i]);
                printf(", ");
            }
            indent--;
            print_indent();
            printf("]");
            indent--;
            print_indent();
            printf(")");
            break;

        case AST_BLOCK:
            printf("AST_BLOCK(");
            indent++;
            print_indent();
            printf("[");
            indent++;
            for (int i = 0; i < ast->as.compound.size; i++) {
                print_indent();
                ast_dump(ast->as.compound.nodes[i]);
                printf(", ");
            }
            indent--;
            print_indent();
            printf("]");
            indent--;
            print_indent();
            printf(")");
            break;

        case AST_VARIABLE_DECLARATION:
            printf("VAR_DECLARATION(");
            indent++;
            print_indent();
            printf("%d,", ast->as.var_decl.type);
            print_indent();
            printf("%s,", ast->as.var_decl.name);
            print_indent();
            ast_dump(ast->as.var_decl.value);
            indent--;
            print_indent();
            printf(")");
            break;

        case AST_IF_STATEMENT:
            printf("IF_STATEMENT(");
            indent++;
            print_indent();
            printf("(");
            ast_dump(ast->as.if_statement.condition);
            printf("),");

            print_indent();
            printf("[");
            indent++;

            for (int i = 0; i < ast->as.if_statement.size; i++) {
                print_indent();
                ast_dump(ast->as.if_statement.nodes[i]);
                printf(", ");
            }
            indent--;
            print_indent();
            printf("]");
            break;

        case AST_BIN_OP:
            printf("BIN_OP(");
            indent++;
            print_indent();

            printf("(");
            ast_dump(ast->as.bin_op.left);
            printf(")");

            print_indent();
            printf("%s", binary_operator(ast->as.bin_op.op));
            print_indent();

            printf("(");
            ast_dump(ast->as.bin_op.right);
            printf(")");

            indent--;
            print_indent();
            printf(")");
            break;
        case AST_UNARY_OP:
            printf("UNARY_OP(");
            indent++;
            print_indent();
            printf("(");
            printf("%s) ", binary_operator(ast->as.unary_op.op));
            print_indent();
            ast_dump(ast->as.unary_op.operand);
            indent--;
            print_indent();
            printf(")");
            break;

        case AST_INTEGER:
            printf("AST_INTEGER(%d)", ast->as.integer.value);
            break;
        case AST_DECIMAL:
            printf("AST_DECIMAL(%f)", ast->as.decimal.value);
            break;
        case AST_STRING:
            printf("AST_STRING('%s')", ast->as.string.value);
            break;

        case AST_VARIABLE: {
            printf("AST_VARIABLE('%s')", ast->as.variable.name);
            break;
        }
        default:
            printf("%s ast_dump() not yet formatted!\n", asttype_to_string(ast->type));
            break;
    }
}

AST *ast_no_op() {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_NO_OP;
    return ast;
}

AST *ast_compound(AST **nodes, size_t size) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_COMPOUND;
    ast->as.compound.nodes = nodes;
    ast->as.compound.size = size;
    return ast;
}

AST *ast_block(AST **nodes, size_t size) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_BLOCK;
    ast->as.block.nodes = nodes;
    ast->as.block.size = size;
    return ast;
}

AST *ast_var_decl(KopfDataType type, char *name, AST *value) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_VARIABLE_DECLARATION;
    ast->as.var_decl.type = type;
    ast->as.var_decl.name = name;
    ast->as.var_decl.value = value;
    return ast;
}

AST *ast_if_statement(AST *condition, AST **nodes, size_t size) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_IF_STATEMENT;
    ast->as.if_statement.condition = condition;
    ast->as.if_statement.nodes = nodes;
    ast->as.if_statement.size = size;
    return ast;
}

AST *ast_bin_op(AST *left, TokenType op, AST *right) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_BIN_OP;
    ast->as.bin_op.left = left;
    ast->as.bin_op.op = op;
    ast->as.bin_op.right = right;
    return ast;
}

AST *ast_unary_op(AST *operand, TokenType op) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_UNARY_OP;
    ast->as.unary_op.operand = operand;
    ast->as.unary_op.op = op;
    return ast;
}

AST *ast_integer(int value) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_INTEGER;
    ast->as.integer.value = value;
    return ast;
}

AST *ast_decimal(double value) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_DECIMAL;
    ast->as.decimal.value = value;
    return ast;
}

AST *ast_string(char *value) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_STRING;
    ast->as.string.value = value;
    return ast;
}

AST* ast_variable(char* name) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_VARIABLE;
    ast->as.variable.name = name;
    return ast;
}
