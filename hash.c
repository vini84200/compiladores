//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hashCreate(HashTable *table, int size) {
    table->size = size;
    table->entries = (HashEntry **) calloc(size, sizeof(HashEntry *));
}
void hashPrint(HashTable *table) {
    int i;
    HashEntry *entry;
    printf("Table -- %d elements, %d collisions\n", table->elements,
           table->collisions);
    for (i = 0; i < table->size; i++) {
        for (entry = table->entries[i]; entry != NULL; entry = entry->next) {
            printf("symbolTable[%d] = %s (type %d)\n", i, entry->value, entry->type);
        }
    }
}

HashEntry *hashInsert(HashTable *table, int type, char *value) {
    HashEntry *newEntry;
    table->elements++;

    int address = hashAddress(value, table->size);
    HashEntry *originalAtAddress = table->entries[address];
    if (originalAtAddress != NULL) {
        // Check if the value is already in the table
        for (HashEntry *entry = originalAtAddress; entry != NULL;
             entry = entry->next) {
            if (strcmp(entry->value, value) == 0) {
                // Value already in the table
                return entry;
            }
        }

        // If not, is a collision
        table->collisions++;
    }

    // Insert the new entry
    newEntry = (HashEntry *) calloc(1, sizeof(HashEntry));
    newEntry->type = type;
    newEntry->value = calloc(strlen(value) + 1, sizeof(char));
    newEntry->next = originalAtAddress;
    strcpy(newEntry->value, value);

    table->entries[address] = newEntry;
    return newEntry;
}

int hashAddress(char *text, int size) {
    int address = 1;
    for (int i = 0; i < strlen(text); i++) {
        address = (address * text[i]) % size + 1;
    }
    return address - 1;
}

HashEntry *hashFind(HashTable *table, char *value) {
    int address = hashAddress(value, table->size);

    if (table->entries[address] != NULL) {
        HashEntry *entry;
        for (entry = table->entries[address]; entry != NULL; entry = entry->next) {
            if (strcmp(entry->value, value) == 0) {
                return entry;
            }
        }
    }

    return NULL;
}
