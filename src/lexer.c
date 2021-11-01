#include "lexer.h"
#include "pch.h"
#include "debug/timer.h"

static bool is_ascii_char(char ch);
static bool is_whitespace(char ch);
static bool is_digit(char ch);

static Token get_token(Lexer *lexer);
static Token *get_tokens(Lexer *lexer);

static Token get_identifier(Lexer *lexer);
static Token get_number(Lexer *lexer);
static Token get_string_literal(Lexer *lexer);

static void advance(Lexer *lexer);
static void skip_whitespace(Lexer *lexer);
static char peek(Lexer *lexer, int offset);

Lexer *lexer_init() {
    return malloc(sizeof(Lexer));
}

Token *lexer_parse(Lexer *lexer, const char *source) {
    TIMER_START();
    lexer->begin = source;
    lexer->end = source;

    Token *tokens = get_tokens(lexer);
    TIMER_END();
    return tokens;
}

static bool is_ascii_char(char ch) {
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

static bool is_whitespace(char ch) {
    return (ch >= '\b' && ch <= '\r') || (ch == ' ');
}

static bool is_digit(char ch) {
    return (ch >= '0' && ch <= '9');
}

static Token get_token(Lexer *lexer) {
    while (true) {
        lexer->begin = lexer->end;
 
        if (is_whitespace(*lexer->end))
            skip_whitespace(lexer);

        if (is_digit(*lexer->end))
            return get_number(lexer);
        if (is_ascii_char(*lexer->end))
            return get_identifier(lexer);
        if(*lexer->end == '"')
            return get_string_literal(lexer);

        switch (*lexer->end) {
            // Arithmetic
            case '+':
                advance(lexer);
                return token_create(TOKEN_PLUS, lexer->begin, lexer->end - lexer->begin);
            case '-':
                advance(lexer);
                return token_create(TOKEN_MINUS, lexer->begin, lexer->end - lexer->begin);
            case '*':
                advance(lexer);
                return token_create(TOKEN_ASTERISK, lexer->begin, lexer->end - lexer->begin);
            case '/':
                advance(lexer);
                return token_create(TOKEN_SLASH, lexer->begin, lexer->end - lexer->begin);

            // Assignment & equality
            case '=':
                advance(lexer);
                if(peek(lexer, 1) == '=') {
                    advance(lexer);
                    return token_create(TOKEN_EQ_EQ, lexer->begin, 2);
                }
                return token_create(TOKEN_EQ, lexer->begin, 1);
            case '!':
                advance(lexer);
                switch(peek(lexer, 1)) {
                    case '=':
                        advance(lexer);
                        return token_create(TOKEN_NOT_EQ, lexer->begin, 2);
                }
                return token_create(TOKEN_NOT, lexer->begin, 1);

            // Access and statement stuff
            case '.':
                advance(lexer);
                return token_create(TOKEN_DOT, lexer->begin, 1);
            case ',':
                advance(lexer);
                return token_create(TOKEN_COMMA, lexer->begin, 1);
            case ':':
                advance(lexer);
                return token_create(TOKEN_COLON, lexer->begin, 1);
            case ';':
                advance(lexer);
                return token_create(TOKEN_SEMICOLON, lexer->begin, 1);
            
            // Braces & blocks
            case '(':
                advance(lexer);
                return token_create(TOKEN_LPAREN, lexer->begin, 1);
            case ')':
                advance(lexer);
                return token_create(TOKEN_RPAREN, lexer->begin, 1);
            case '{':
                advance(lexer);
                return token_create(TOKEN_LBRACE, lexer->begin, 1);
            case '}':
                advance(lexer);
                return token_create(TOKEN_RBRACE, lexer->begin, 1);

            case '\0':
                advance(lexer);
                return token_create(END_OF_FILE, "EOF", 3);
            default:
                printf("Unknown char: %c\n", *lexer->end);
                advance(lexer);
        }
    }
}

static Token *get_tokens(Lexer *lexer) {
    Token *tokens = malloc(sizeof(Token));

    Token token = get_token(lexer);
    size_t index = 0;
    tokens[index++] = token;
    while (token.type != END_OF_FILE) {
        tokens = realloc(tokens, (index + 1) * sizeof(Token));
        token = get_token(lexer);
        tokens[index++] = token;
    }
    return tokens;
}

static Token get_identifier(Lexer *lexer) {
    while (is_ascii_char(*lexer->end)) {
        advance(lexer);
    }
    size_t length = lexer->end - lexer->begin;
    TokenType type = TOKEN_IDENTIFIER;

    if(memcmp(lexer->begin, "int", length) == 0)
        type = TOKEN_KW_TYPE;
    else if (memcmp(lexer->begin, "if", 2) == 0)
        type = TOKEN_KW_IF;


    return token_create(type, lexer->begin, length);
}

static Token get_number(Lexer *lexer) {
    while (is_digit(*lexer->end)) {
        advance(lexer);
    }
    return token_create(TOKEN_INTEGER, lexer->begin, lexer->end - lexer->begin);
}

static Token get_string_literal(Lexer *lexer) {
    lexer->end++;
    lexer->begin = lexer->end;
    while (*lexer->end != '"') {
        advance(lexer);
    }
    Token token = token_create(TOKEN_STRING, lexer->begin, lexer->end - lexer->begin);   
    advance(lexer);
    return token;
}

static void advance(Lexer *lexer) {
    lexer->end++;
}

static void skip_whitespace(Lexer *lexer) {
    while (is_whitespace(*lexer->begin)) {
        lexer->begin++;
    }
    lexer->end = lexer->begin;
}

static char peek(Lexer *lexer, int offset) {
    return lexer->end[offset];
}
