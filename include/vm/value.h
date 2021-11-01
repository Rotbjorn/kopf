#pragma once
#include "pch.h"

#define ENUMERATE_VALUES(O) \
    O(VAL_STRING) \
    O(VAL_BOOLEAN) \
    O(VAL_INTEGER) \
    O(VAL_DECIMAL)

#define __VALUE(value) \
    value, 
typedef enum ValueType {
ENUMERATE_VALUES(__VALUE)
} ValueType;
#undef __VALUE

const char* valuetype_to_string(ValueType type);


typedef struct {
    ValueType type;
    union {
        uint8_t boolean;
        int integer;
        double decimal;
    } as;
} Value;

Value boolean_value(uint8_t value);
Value integer_value(int value);
Value decimal_value(double value);

int8_t is_boolean(const Value* value);
int8_t is_integer(const Value* value);
int8_t is_decimal(const Value* value);

int8_t value_compare(const Value* v1, const Value* v2);

void value_dump(const Value* value);
