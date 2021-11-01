#include "token.h"

#define __TOKENS(token) \
    case token:\
        return #token;
const char* tokentype_to_string(TokenType type) {
    switch(type) {
        ENUMERATE_TOKENS(__TOKENS)
    }
    return "UNKNOWN";
}
#undef __TOKENS

Token token_create(TokenType type, const char* value, size_t length) {
    Token token;
    token.type = type;
    token.value = value;
    token.length = length;
    return token;
}

char* token_value_copy(Token* token) {
    char* string = malloc(token->length + 1);
    memcpy(string, token->value, token->length);
    string[token->length] = '\0';
    return string;
}

void token_print(Token* token) {
    const char* value = token_value_copy(token);
    printf("Token(type='%s',value='%s')\n", tokentype_to_string(token->type), value);
    free((void*)value);
}
