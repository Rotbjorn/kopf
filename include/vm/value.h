#pragma once
#include "pch.h"

#define ENUMERATE_VALUES(O) \
    O(VAL_STRING)           \
    O(VAL_BOOLEAN)          \
    O(VAL_INTEGER)          \
    O(VAL_DECIMAL)

#define __VALUE(value) \
    value,
typedef enum ValueType {
    ENUMERATE_VALUES(__VALUE)
} ValueType;
#undef __VALUE

const char *valuetype_to_string(ValueType type);

typedef struct Value {
    ValueType type;
    union {
        struct {
            char *chars;
            int length;
        } string;
        uint8_t boolean;
        int integer;
        double decimal;
    } as;
} Value;

Value string_value(const char *value);
Value boolean_value(bool value);
Value integer_value(int value);
Value decimal_value(double value);

bool is_string(const Value *value);
bool is_boolean(const Value *value);
bool is_integer(const Value *value);
bool is_decimal(const Value *value);

int8_t value_compare(const Value *v1, const Value *v2);
Value *value_copy(const Value *value);

void value_dump(const Value *value);
