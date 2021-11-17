#include "vm/value.h"
#include "debug/color.h"

const char *valuetype_to_string(ValueType type) {
#define __VALUE(value) \
    case value:        \
        return #value;
    switch (type) {
        ENUMERATE_VALUES(__VALUE)
    }
    printf("THIS BOI HERE IS SUS AF: %d\n", type);
    return "UNKNOWN";
#undef __VALUE
}

Value string_value(const char *value) {
    Value val;
    val.type = VAL_STRING;
    int length = strlen(value);
    val.as.string.length = length;
    val.as.string.chars = malloc(length);
    strncpy(val.as.string.chars, value, length);
    return val;
}

Value boolean_value(bool value) {
    return (Value){
        .type = VAL_BOOLEAN,
        .as.boolean = value};
}
Value integer_value(int value) {
    return (Value){
        .type = VAL_INTEGER,
        .as.integer = value};
}
Value decimal_value(double value) {
    return (Value){
        .type = VAL_DECIMAL,
        .as.decimal = value};
}

bool is_string(const Value *value) {
    return value->type == VAL_STRING;
}
bool is_boolean(const Value *value) {
    return value->type == VAL_BOOLEAN;
}

bool is_integer(const Value *value) {
    return value->type == VAL_INTEGER;
}

bool is_decimal(const Value *value) {
    return value->type == VAL_DECIMAL;
}

int8_t value_compare(const Value *v1, const Value *v2) {
    if (v1->type != v2->type)
        return 0;

    switch (v1->type) {
        case VAL_STRING:
            printf("TODO: STRING COMPARE SUPPORT\n");
            exit(-69);
        case VAL_BOOLEAN:
            return (v1->as.boolean == v2->as.boolean);
        case VAL_INTEGER:
            return (v1->as.integer == v2->as.integer);
        case VAL_DECIMAL:
            return (v1->as.decimal == v2->as.decimal);
    }
    return false;
}

Value *value_copy(const Value *value) {
    Value *val = malloc(sizeof(struct Value));
    memcpy(val, value, sizeof(struct Value));
    return val;
}

void value_dump(const Value *value) {
    printf(
        WHITE BOLD "Value" RESET
            GRAY "(" RESET
                DARK_GRAY "type=" RESET
                   "'%s'" GRAY ", " RESET
                       DARK_GRAY "value=" RESET
                   "'",
        valuetype_to_string(value->type));
    switch (value->type) {
        case VAL_STRING:
            printf("length: %d')", value->as.string.length);
            break;
        case VAL_BOOLEAN:
            printf("%s'", (value->as.boolean >= 1 ? "true" : "false"));
            break;
        case VAL_INTEGER:
            printf("%d'", value->as.integer);
            break;
        case VAL_DECIMAL:
            printf("%f'", value->as.decimal);
            break;
    }
    printf(GRAY ")" RESET);
}
