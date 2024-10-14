#pragma once
#include "token.h"
#include "datatypes.h"

#define ENUMERATE_AST(O)        \
    O(AST_NO_OP)                \
                                \
    /*TODO: change to module?*/ \
    O(AST_COMPOUND)             \
    O(AST_BLOCK)                \
                                \
    O(AST_VARIABLE_DECLARATION) \
                                \
    O(AST_IF_STATEMENT)         \
                                \
    O(AST_UNARY_OP)             \
    O(AST_BIN_OP)               \
                                \
    O(AST_INTEGER)              \
    O(AST_DECIMAL)              \
    O(AST_STRING)               \
                                \
    O(AST_VARIABLE)\
    O(AST_SIZE)

#define __AST(ast) \
    ast,

typedef enum ASTType {
    ENUMERATE_AST(__AST)
} ASTType;
#undef __AST

typedef struct AST AST;

struct AST {
    ASTType type;
    union {
        struct {
        } no_op;

        struct {
            AST **nodes;
            size_t size;
        } compound;

        struct {
            AST **nodes;
            size_t size;
        } block;

        struct {
            KopfDataType type;
            char *name;
            AST *value;
        } var_decl;

        struct {
            AST *condition;
            AST **nodes;
            size_t size;
        } if_statement;

        struct {
            AST *left;
            TokenType op;
            AST *right;
        } bin_op;
        struct {
            AST *operand;
            TokenType op;
        } unary_op;

        struct {
            int value;
        } integer;
        struct {
            double value;
        } decimal;
        struct {
            char *value;
        } string;

        struct {
            char *name;
        } variable;
    } as;
};

const char *asttype_to_string(ASTType);
void ast_free(AST *ast);
void ast_dump(AST *ast);
void ast_dump_dot(AST *ast, const char* file_path);

AST *ast_no_op();

AST *ast_compound(AST **nodes, size_t size);
AST *ast_block(AST **nodes, size_t size);

AST *ast_var_decl(KopfDataType type, char *name, AST *value);

AST *ast_if_statement(AST *condition, AST **nodes, size_t size);

AST *ast_bin_op(AST *left, TokenType op, AST *right);
AST *ast_unary_op(AST *operand, TokenType op);

AST *ast_integer(int value);
AST *ast_decimal(double value);
AST *ast_string(char *value);

AST *ast_variable(char *name);
