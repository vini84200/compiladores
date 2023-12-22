#ifndef TAC_HEADER
#define TAC_HEADER

#include "hash.h"
typedef enum TacOp_t {
    TAC_SYMBOL,
    TAC_ADD
} TacOp;

#define TAC_SRC_NUMBER 2

typedef struct Tac_t {
    TacOp op;
    HashEntry *dst;
    HashEntry *src[TAC_SRC_NUMBER];
} Tac;

Tac createTac(TacOp op, HashEntry *dst, HashEntry *src1, HashEntry *src2);
Tac createSymbolTac(HashEntry *symbol);

typedef struct TacListItem_t {
    Tac tac;
    struct TacListItem_t *next;
    struct TacListItem_t *prev;
} TacListItem;

typedef struct TacList_t {
    int size;
    TacListItem *head;
    TacListItem *last;
} TacList;

TacList *createTacList();
void destroyTacList(TacList *tacList);

void appendTacList(TacList *tl, Tac t);
TacList *joinTacList(TacList *a, TacList *b);

// Iterator
typedef struct TacIterator_t {
    TacList *tacList;
    TacListItem *current;
    int index;
} TacIterator;

TacIterator createTacForwardIterator(TacList *list);
TacIterator createTacBackwardIterator(TacList *list);
Tac *TacFIterNext(TacIterator *iter);
Tac *TacFIterPeek(TacIterator *iter);
Tac *TacFIterBack(TacIterator *iter);
bool TacFIterDone(TacIterator *iter);
bool TacFIterFirst(TacIterator *iter);

void printTACList(TacList *list);
void printCode(TacList *list);


#endif// !TAC_HEADER
