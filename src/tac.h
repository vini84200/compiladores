#ifndef TAC_HEADER
#define TAC_HEADER

#include "hash.h"
#include <stdio.h>

#define TAC_OPT_Gen(F, S) \
    F(TAC_SYMBOL)         \
    S F(TAC_MOVE)         \
    S F(TAC_LABEL)        \
    S F(TAC_JUMP)         \
    S F(TAC_ADD)          \
    S F(TAC_SUB)          \
    S F(TAC_MUL)          \
    S F(TAC_DIV)          \
    S F(TAC_LEQ)          \
    S F(TAC_LT)           \
    S F(TAC_EQ)           \
    S F(TAC_NEQ)          \
    S F(TAC_GEQ)          \
    S F(TAC_GT)           \
    S F(TAC_AND)          \
    S F(TAC_OR)           \
    S F(TAC_NOT)          \
    S F(TAC_IFZ)          \
    S F(TAC_BEGFUN)       \
    S F(TAC_ENDFUN)       \
    S F(TAC_ARG)          \
    S F(TAC_CALL)         \
    S F(TAC_RET)          \
    S F(TAC_PRINT)        \
    S F(TAC_READ)         \
    S F(TAC_MOVE_ARRAY)   \
    S F(TAC_LOAD_ARRAY)   \
    S F(TAC_COMMENT)      \
    S F(TAC_CONSUMED_TEMP)\
    S F(TAC_WILL_CALL)


#define COMMA ,
#define ID(x) x

typedef enum TacOp_t {
    TAC_OPT_Gen(ID, COMMA),
    TAC_MAX
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
void printCode(TacList *list, FILE *fileOut);


#endif// !TAC_HEADER
