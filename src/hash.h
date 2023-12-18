//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#ifndef SCANNER_TEST_HASH_H
#define SCANNER_TEST_HASH_H
#include "symbols.h"
#include "types.h"

#include <stdbool.h>

typedef struct HashEntry_t {
    SymbolType type;
    char *value;
    Identifier *identifier;
    bool implemented;
    struct HashEntry_t *nextInHashBucket;
    struct HashEntry_t *nextOfTheSameType;
} HashEntry;

typedef struct {
    HashEntry **entries;
    int size;
    int collisions;
    int elements;
    HashEntry *heads[LAST_SYMBOL + 1];
    HashEntry *tails[LAST_SYMBOL + 1];
} HashTable;

int hashAddress(char *text, int size);
HashEntry *hashInsert(HashTable *table, int type, char *value);
HashEntry *hashFind(HashTable *table, char *value);
void hashCreate(HashTable *table, int size);
void hashPrint(HashTable *table);

typedef struct {
    HashTable *table;
    HashEntry *next;
} SymbolIterator;

SymbolIterator *createSymbolIterator(HashTable *table, SymbolType type);
HashEntry *getNextSymbol(SymbolIterator *iterator);
bool SymbolIteratorDone(SymbolIterator *iterator);
void destroySymbolIterator(SymbolIterator *iterator);


#endif// SCANNER_TEST_HASH_H
