#include "ast.h"
#include <rotbjorn/skrivarn.h>

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

void ast_free(AST *ast) {
    static_assert(AST_SIZE == 11, "Exhaustive handling");
    skrivarn_infof("Freeing: %s", asttype_to_string(ast->type));
    switch (ast->type) {
        case AST_NO_OP:
            break;
        case AST_COMPOUND:
            for (int i = 0; i < ast->as.compound.size; i++) {
                ast_free(ast->as.compound.nodes[i]);
            }
            free((void *)ast->as.compound.nodes);
            break;
        case AST_BLOCK:
            for (int i = 0; i < ast->as.block.size; i++) {
                ast_free(ast->as.block.nodes[i]);
            }
            free((void *)ast->as.block.nodes);
            break;
        case AST_VARIABLE_DECLARATION:
            ast_free(ast->as.var_decl.value);
            free(ast->as.var_decl.name);
            break;
        case AST_IF_STATEMENT:
            ast_free((void *)ast->as.if_statement.condition);
            for (int i = 0; i < ast->as.if_statement.size; i++) {
                ast_free(ast->as.if_statement.nodes[i]);
            }
            free((void *)ast->as.if_statement.nodes);
            break;
        case AST_UNARY_OP:
            ast_free(ast->as.unary_op.operand);
            break;
        case AST_BIN_OP:
            ast_free(ast->as.bin_op.left);
            ast_free(ast->as.bin_op.right);
            break;
        case AST_INTEGER:
            break;
        case AST_DECIMAL:
            break;
        case AST_STRING:
            free((void *)ast->as.string.value);
            break;
        case AST_VARIABLE:
            free((void *)ast->as.variable.name);
            break;
        default:
            skrivarn_error("unreachable");
            exit(1);
            break;
    }
    free((void *)ast);
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

static int _ast_dump_dot_impl(AST *ast, int *count, FILE *sink) {
    switch (ast->type) {
        case AST_COMPOUND: {
            int start = *count;
            fprintf(sink, "Node_%d [label = \"COMPOUND\"]\n", *count);
            (*count)++;
            for (int i = 0; i < ast->as.compound.size; i++) {
                AST *node = ast->as.compound.nodes[i];
                int target = _ast_dump_dot_impl(node, count, sink);
                fprintf(sink, "Node_%d -> Node_%d\n", start, target);
            }
            return start;
        }
        case AST_BLOCK: {
            int start = *count;
            fprintf(sink, "Node_%d [label = \"BLOCK\"]\n", *count);
            (*count)++;
            for (int i = 0; i < ast->as.block.size; i++) {
                AST *node = ast->as.block.nodes[i];
                int target = _ast_dump_dot_impl(node, count, sink);
                fprintf(sink, "Node_%d -> Node_%d\n", start, target);
            }
            return start;
        }
        case AST_VARIABLE_DECLARATION: {

            int start = *count;
            fprintf(sink, "Node_%d [label = \"var_decl %s\"]\n", *count, ast->as.var_decl.name);
            (*count)++;
            int value = _ast_dump_dot_impl(ast->as.var_decl.value, count, sink);
            fprintf(sink, "Node_%d -> Node_%d\n", start, value);

            return start;
        }

        case AST_IF_STATEMENT:
            fprintf(sink, "Node_%d [label = \"IF_STATEMENT\"]\n", *count);
            return (*count)++;

        case AST_UNARY_OP:
            fprintf(sink, "Node_%d [label = \"UNARY_OP\"]\n", *count);
            return (*count)++;

        case AST_BIN_OP: {
            int start = *count;
            fprintf(sink, "Node_%d [label = \"%s\"]\n", *count, tokentype_to_string(ast->as.bin_op.op));
            (*count)++;

            int lhs = _ast_dump_dot_impl(ast->as.bin_op.left, count, sink);
            int rhs = _ast_dump_dot_impl(ast->as.bin_op.right, count, sink);

            fprintf(sink, "Node_%d -> { Node_%d Node_%d }\n", start, lhs, rhs);

            return start;
        }

        case AST_INTEGER:
            fprintf(sink, "Node_%d [label = \"%d\"]\n", *count, ast->as.integer.value);
            return (*count)++;

        case AST_DECIMAL:
            fprintf(sink, "Node_%d [label = \"%lf\"]\n", *count, ast->as.decimal.value);
            return (*count)++;

        case AST_STRING:
            fprintf(sink, "Node_%d [label = \"\"%s\"\"]\n", *count, ast->as.string.value);
            return (*count)++;

        case AST_VARIABLE: {
            int start = *count;
            fprintf(sink, "Node_%d [label = \"var: %s\"]\n", *count, ast->as.variable.name);
            (*count)++;
            return start;
        }

        default:
            skrivarn_error("Unreachable");
            exit(1);
    }
}

void ast_dump_dot(AST *ast, const char *file_path) {
    int count = 0;
    FILE *file = fopen(file_path, "w");
    if (!file) {
        skrivarn_errorf("Couldn't create %s", file_path);
        return;
    }

    fprintf(file, "digraph AST {\n");
    fprintf(file, "fontname = \"JetBrains Mono\"\n");
    fprintf(file, "label = \"kopf - Abstract Syntax Tree\"\n");
    fprintf(file, "node [ shape = square fontname = \"JetBrains Mono\" fontsize=5]");
    _ast_dump_dot_impl(ast, &count, file);
    fprintf(file, "}\n");

    fclose(file);
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

AST *ast_variable(char *name) {
    AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_VARIABLE;
    ast->as.variable.name = name;
    return ast;
}
