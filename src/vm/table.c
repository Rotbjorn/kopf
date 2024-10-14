#include "vm/table.h"
#include <rotbjorn/skrivarn.h>
#include "datatypes.h"

// TODO: Optimize this to not calculate hash every time
// and not use strcmp... (make sure there are only unique strings (check by pointer value))

static Entry *entry_create(const char *key, void *value) {
    Entry *entry = malloc(sizeof(struct Entry));

    entry->key = malloc(strlen(key) + 1);

    strcpy(entry->key, key);

    entry->value = value;

    return entry;
}
static uint32_t kopf_hash_string(const char *key);

static Entry **find_entry(Table *table, const char *key, uint32_t hash) {
    uint32_t index = hash % KOPF_TABLE_SIZE;
    for (;;) {
        Entry **entry = &table->entries[index];
        if (entry[0] == NULL) {
            return entry;
        } else if (strcmp(entry[0]->key, key) == 0) {
            return entry;
        }

        index = (index + 1) % KOPF_TABLE_SIZE;
    }
}

Table *table_init() {
    Table *table = malloc(sizeof(struct Table));
    table->entries = (Entry**) malloc(sizeof(Entry *) * KOPF_TABLE_SIZE);

    for (int i = 0; i < KOPF_TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }

    return table;
}

bool table_set(Table *table, const char *key, void *value) {
    uint32_t hash = kopf_hash_string(key);
    Entry **entry = find_entry(table, key, hash);

    if (entry[0] == NULL) {
        entry[0] = entry_create(key, value);
        return true;
    }

    entry[0]->value = value;

    return false;
}

void *table_get(Table *table, const char *key) {
    Entry **entry = find_entry(table, key, kopf_hash_string(key));

    if (entry[0] == NULL) {
        return NULL;
    }

    void *value = entry[0]->value;
    return value;
}

bool table_delete(Table *table, const char *key) {
    Entry **entry = find_entry(table, key, kopf_hash_string(key));

    if (entry[0] == NULL)
        return false;

    free(entry[0]->key);
    free(entry[0]);

    entry[0] = NULL;
    return true;
}

void table_dump(Table *table) {
    for (int i = 0; i < KOPF_TABLE_SIZE; i++) {
        Entry *entry = table->entries[i];

        if (entry == NULL) {
            continue;
        }

        skrivarn_infof("Entry(key='%s', ptr='%x')", entry->key, entry->value);
    }
}
void table_free(Table **table) {
    for (int i = 0; i < KOPF_TABLE_SIZE; i++) {
        Entry *entry = (*table)->entries[i]; 

        if (entry != NULL) {
            skrivarn_warnf("Freed key: %s", entry->key);
            free(entry->key);
            free(entry->value);
            free(entry);
        }
    }
    free((*table)->entries);
    free(*table);

    table = NULL;
}

uint32_t kopf_hash_string(const char *key) {
    uint32_t hash = 2166136261u;
    for (int i = 0; key[i] != '\0'; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}
