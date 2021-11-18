#include "parser.h"
#include "debug/timer.h"
#include <rotbjorn/skrivarn.h>
#include "io.h"
#include "lexer.h"

typedef enum Precedence {
    PREC_NONE,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY
} Precedence;

typedef AST *(*PrefixFunc)(Parser *);
typedef AST *(*InfixFunc)(Parser *);

static AST* parser_parse(Parser* parser);

static AST *parser_declaration(Parser *parser);

// Declarations
static AST *parser_variable_declaration(Parser *parser);
static AST *parser_include(Parser *parser);

// Statements
static AST *parser_statement(Parser *parser);
static AST *parser_if_statement(Parser *parser);
static AST *parser_block(Parser *parser);

// Expressions
static AST *parser_expression(Parser *parser);
static AST *parser_precedence(Parser *parser, Precedence prec);

static PrefixFunc parser_prefix(Parser *parser);
static InfixFunc parser_infix(Parser *parser);

static AST *parser_integer(Parser *parser);
static AST *parser_decimal(Parser *parser);
static AST *parser_string(Parser *parser);

static AST *parser_variable(Parser *parser);

static AST *parser_unary(Parser *parser);

static AST *parser_addition(Parser *parser);
static AST *parser_subtraction(Parser *parser);
static AST *parser_multiplication(Parser *parser);
static AST *parser_division(Parser *parser);
static AST *parser_grouping(Parser *parser);

static void parser_eat(Parser *parser, TokenType expected);
static Precedence token_precedence(TokenType type);

AST *parser_generate_ast(Parser *parser) {
    return parser_parse(parser);
}

Parser *parser_init(Token *token) {
    Parser *parser = malloc(sizeof(Parser));
    parser->index = 0;
    parser->token = token;
    return parser;
}

AST *parser_parse(Parser *parser) {
    AST **nodes = malloc(sizeof(struct AST *));
    size_t size = 1;
    nodes[0] = parser_declaration(parser);

    while (parser->token->type != END_OF_FILE) {
        AST* node = parser_declaration(parser);
        if (node == NULL || node->type == AST_NO_OP) continue;

        nodes = realloc(nodes, sizeof(struct AST *) * ++size);
        nodes[size - 1] = node;
    }
    AST *compound = ast_compound(nodes, size);

    return compound;
}

static AST *parser_declaration(Parser *parser) {
    if (parser->token->type == TOKEN_TYPE_INT
            || parser->token->type == TOKEN_TYPE_DECIMAL
            || parser->token->type == TOKEN_TYPE_STRING) {
        return parser_variable_declaration(parser);
    } else if (parser->token->type == TOKEN_KW_INCLUDE) {
        return parser_include(parser);
    }
    return parser_statement(parser);
}

static AST *parser_variable_declaration(Parser *parser) {
    TokenType type = parser->token->type;
    parser_eat(parser, type);
    char *name = token_value_copy(parser->token);
    parser_eat(parser, TOKEN_IDENTIFIER);
    parser_eat(parser, TOKEN_EQ);
    AST *value = parser_expression(parser);
    parser_eat(parser, TOKEN_SEMICOLON);

    KopfDataType data_type;
    switch(type) {
        case TOKEN_TYPE_INT:
            data_type = KOPF_DATA_TYPE_INT;
            break;
        case TOKEN_TYPE_DECIMAL:
            data_type = KOPF_DATA_TYPE_DECIMAL;
            break;
        case TOKEN_TYPE_STRING:
            data_type = KOPF_DATA_TYPE_STRING;
            break;
        default:
            skrivarn_errorf("Type is wrong...: %s", tokentype_to_string(type));
            break;
        
    }

    return ast_var_decl(data_type, name, value);
}


static AST *parser_include(Parser *parser) {
    parser_eat(parser, TOKEN_KW_INCLUDE);
    parser_eat(parser, TOKEN_LBRACE);

    skrivarn_error("Here? 1");
    const char* file_name = token_value_copy(parser->token);

    // TODO: Change this later...
    parser_eat(parser, TOKEN_STRING);

    Lexer *lexer = lexer_init();

    skrivarn_error("Here? 2");
    Token* tokens = lexer_parse(lexer, read_file(file_name));
    
    skrivarn_error("Here? 3");
    Token* backup = parser->token;

    parser->token = tokens;

    skrivarn_error("Here? 4");
    // TODO: cyclic dependencies... keep track of imported files?
    AST* root = parser_parse(parser);

    parser->token = backup;

    skrivarn_error("Here? 5");
    parser_eat(parser, TOKEN_RBRACE);

    return root;
}

static AST *parser_statement(Parser *parser) {
    switch (parser->token->type) {
        case TOKEN_KW_IF:
            return parser_if_statement(parser);
        case TOKEN_LBRACE:
            return parser_block(parser);
        default:
            return parser_expression(parser);
    }
    return parser_expression(parser);
}

static AST *parser_if_statement(Parser *parser) {
    parser_eat(parser, TOKEN_KW_IF);
    parser_eat(parser, TOKEN_LPAREN);
    AST *condition = parser_expression(parser);
    parser_eat(parser, TOKEN_RPAREN);

    parser_eat(parser, TOKEN_LBRACE);

    AST **nodes = malloc(sizeof(struct AST *));
    size_t size = 0;

    if (parser->token->type != TOKEN_RBRACE) {
        nodes[size++] = parser_declaration(parser);
        printf("DONE HERE BRO: %s\n", tokentype_to_string(parser->token->type));
    }

    while (parser->token->type != TOKEN_RBRACE) {
        nodes = realloc(nodes, sizeof(struct AST *) * ++size);
        nodes[size - 1] = parser_declaration(parser);
    }

    parser_eat(parser, TOKEN_RBRACE);

    AST *ast = ast_if_statement(condition, nodes, size);
    return ast;
}

static AST *parser_block(Parser *parser) {
    parser_eat(parser, TOKEN_LBRACE);

    AST **nodes = malloc(sizeof(struct AST *));
    size_t size = 1;
    nodes[0] = parser_declaration(parser);

    while (parser->token->type != END_OF_FILE && parser->token->type != TOKEN_RBRACE) {
        AST* node = parser_declaration(parser);
        if (node == NULL || node->type == AST_NO_OP) continue;

        nodes = realloc(nodes, sizeof(struct AST *) * ++size);
        nodes[size - 1] = node;
    }
    parser_eat(parser, TOKEN_RBRACE);

    AST *ast = ast_block(nodes, size);
    return ast;
}

static AST *parser_expression(Parser *parser) {
    return parser_precedence(parser, PREC_NONE);
}

static AST *parser_precedence(Parser *parser, Precedence prec) {
    PrefixFunc prefix = parser_prefix(parser);

    if (!prefix) {
        const char *value = token_value_copy(parser->token);
        printf("SUS-PREFIX-ERROR!\nType: %s,\nValue: %s\n", tokentype_to_string(parser->token->type), value);
        free((void *)value);
    }

    AST *node = prefix(parser);

    while (prec <= token_precedence(parser->token->type)) {
        InfixFunc infix = parser_infix(parser);
        if (!infix) {
            break;
        }
        AST *bin_op = infix(parser);
        bin_op->as.bin_op.left = node;
        node = bin_op;
    }
    return node;
}

static PrefixFunc parser_prefix(Parser *parser) {
    switch (parser->token->type) {
        case TOKEN_STRING:
            return parser_string;
        case TOKEN_LPAREN:
            return parser_grouping;
        case TOKEN_INTEGER:
            return parser_integer;
        case TOKEN_DECIMAL:
            return parser_decimal;
        case TOKEN_MINUS:
            return parser_unary;
        case TOKEN_IDENTIFIER:
            return parser_variable;
        default:
            return NULL;
    }
}

static InfixFunc parser_infix(Parser *parser) {
    switch (parser->token->type) {
        case TOKEN_PLUS:
            return parser_addition;
        case TOKEN_MINUS:
            return parser_subtraction;
        case TOKEN_ASTERISK:
            return parser_multiplication;
        case TOKEN_SLASH:
            return parser_division;
        default:
            return NULL;
    }
}

static AST *parser_integer(Parser *parser) {
    const char *str_value = token_value_copy(parser->token);
    int value = atoi(str_value);
    free((void*)str_value);
    parser_eat(parser, TOKEN_INTEGER);
    return ast_integer(value);
}

static AST *parser_decimal(Parser *parser) {
    const char *str_value = token_value_copy(parser->token);
    double value = strtod(str_value, NULL);
    free((void*)str_value);
    parser_eat(parser, TOKEN_DECIMAL);
    return ast_decimal(value);
}

static AST *parser_string(Parser *parser) {
    char *str = token_value_copy(parser->token);
    parser_eat(parser, TOKEN_STRING);
    return ast_string(str);
}

static AST *parser_variable(Parser *parser) {
    char* name =  token_value_copy(parser->token);

    parser_eat(parser, TOKEN_IDENTIFIER);

    return ast_variable(name);
}


static AST *parser_unary(Parser *parser) {
    parser_eat(parser, TOKEN_MINUS);
    AST *operand = parser_precedence(parser, PREC_UNARY);
    return ast_unary_op(operand, TOKEN_MINUS);
}

static AST *parser_addition(Parser *parser) {
    parser_eat(parser, TOKEN_PLUS);
    AST *right = parser_precedence(parser, PREC_FACTOR);
    return ast_bin_op(NULL, TOKEN_PLUS, right);
}

static AST *parser_subtraction(Parser *parser) {
    parser_eat(parser, TOKEN_MINUS);
    AST *right = parser_precedence(parser, PREC_FACTOR);
    return ast_bin_op(NULL, TOKEN_MINUS, right);
}

static AST *parser_multiplication(Parser *parser) {
    parser_eat(parser, TOKEN_ASTERISK);
    AST *right = parser_precedence(parser, PREC_FACTOR + 1);
    return ast_bin_op(NULL, TOKEN_ASTERISK, right);
}

static AST *parser_division(Parser *parser) {
    parser_eat(parser, TOKEN_SLASH);
    AST *right = parser_precedence(parser, PREC_FACTOR + 1);
    return ast_bin_op(NULL, TOKEN_SLASH, right);
}

static AST *parser_grouping(Parser *parser) {
    parser_eat(parser, TOKEN_LPAREN);
    AST *node = parser_expression(parser);
    parser_eat(parser, TOKEN_RPAREN);
    return node;
}

static void parser_eat(Parser *parser, TokenType expected) {
    if (parser->token->type != expected) {
        printf("Error: Expected: '%s', Actual: '%s'\n", tokentype_to_string(expected), tokentype_to_string(parser->token->type));
    }
    parser->token++;
}

static Precedence token_precedence(TokenType type) {
    switch (type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return PREC_TERM;

        case TOKEN_ASTERISK:
        case TOKEN_SLASH:
            return PREC_FACTOR;

        default:
            return PREC_NONE;
    }
}
