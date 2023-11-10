//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#ifndef SCANNER_TEST_HASH_H
#define SCANNER_TEST_HASH_H
#include "symbols.h"

typedef struct HashEntry {
    SymbolType type;
    char *value;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry **entries;
    int size;
    int collisions;
    int elements;
} HashTable;

int hashAddress(char *text, int size);
HashEntry *hashInsert(HashTable *table, int type, char *value);
HashEntry *hashFind(HashTable *table, char *value);
void hashCreate(HashTable *table, int size);
void hashPrint(HashTable *table);

#endif// SCANNER_TEST_HASH_H
