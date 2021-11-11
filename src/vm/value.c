#include "vm/value.h"
#include "debug/color.h"

const char *valuetype_to_string(ValueType type) {
#define __VALUE(value) \
    case value:        \
        return #value;
    switch (type) {
        ENUMERATE_VALUES(__VALUE)
    }
#undef __VALUE
}

Value boolean_value(uint8_t value) {
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

int8_t is_boolean(const Value *value) {
    return value->type == VAL_BOOLEAN;
}

int8_t is_integer(const Value *value) {
    return value->type == VAL_INTEGER;
}

int8_t is_decimal(const Value *value) {
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

void value_dump(const Value *value) {
    printf(
        WHITE BOLD "Value" RESET
        GRAY "(" RESET
        DARK_GRAY "type=" RESET
        "'%s'"
        GRAY ", " RESET
        DARK_GRAY "value=" RESET
        "'", valuetype_to_string(value->type));
    switch (value->type) {
        case VAL_STRING:
            printf("STRING value_dump() not yet done)')");
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
