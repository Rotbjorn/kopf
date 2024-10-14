#pragma once
#include "value.h"
#include "pch.h"

#define KOPF_TABLE_SIZE 512

typedef struct Entry {
    char *key;
    void *value;
} Entry;

typedef struct Table {
    Entry **entries;
} Table;

Table *table_init();
bool table_set(Table *table, const char *key, void *value);
void* table_get(Table *table, const char *key);
bool table_delete(Table *table, const char *key);
void table_dump(Table *table);

void table_free(Table **table);
