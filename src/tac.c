#include "tac.h"
#include <stdio.h>
#include <stdlib.h>

Tac createTac(TacOp op, HashEntry *dst, HashEntry *src1, HashEntry *src2) {
    Tac t = {
            .op = op,
            .dst = dst,
            .src = {src1, src2}};
    return t;
}
Tac createSymbolTac(HashEntry *symbol) {
    return createTac(TAC_SYMBOL, symbol, NULL, NULL);
}

TacList *createTacList() {
    TacList *tl = calloc(1, sizeof(TacList));
    tl->head = NULL;
    tl->last = NULL;
    tl->size = 0;
    return tl;
}
void destroyTacList(TacList *tacList) {
    // TODO: Iterate through all TACs and delete them

    free(tacList);
}

void appendTacList(TacList *tl, Tac t) {
    TacListItem *item = calloc(1, sizeof(TacListItem));
    item->tac = t;
    item->next = NULL;
    item->prev = NULL;

    if (tl->last == NULL) {
        // This is the first tac of the list
        tl->head = item;
        tl->last = item;
        tl->size++;
    } else {
        // Normal append
        tl->last->next = item;
        item->prev = tl->last;
        tl->last = item;
        tl->size++;
    }
}

TacList *joinTacList(TacList *a, TacList *b) {
    if (a == NULL) {
        // a doesn't exist
        return b;// CAN BE NULL
    }
    if (b == NULL || b->head == NULL) {
        // b is empty or doesn't exist
        return a;
    }
    if (a->head == NULL) {
        return b;
    }
    TacList *newList = calloc(1, sizeof(TacList));
    newList->head = a->head;
    a->last->next = b->head;
    b->head->prev = a->last;
    newList->last = b->last;
    newList->size = a->size + b->size;
    return newList;
}

TacIterator createTacForwardIterator(TacList *list) {
    TacIterator iter;
    iter.tacList = list;
    if (list == NULL) {
        iter.current = NULL;
    } else {
        iter.current = list->head;
    }
    iter.index = 0;
    return iter;
}
TacIterator createTacBackwardIterator(TacList *list) {
    TacIterator iter;
    iter.tacList = list;
    iter.current = list->last;
    iter.index = list->size - 1;
    return iter;
}
Tac *TacFIterNext(TacIterator *iter) {
    Tac *curr = &iter->current->tac;
    if (iter->current != NULL) {
        iter->index++;
        iter->current = iter->current->next;
    }
    return curr;
}
Tac *TacFIterPeek(TacIterator *iter) {
    return &iter->current->tac;
}
Tac *TacFIterBack(TacIterator *iter) {
    Tac *curr = &iter->current->tac;
    if (iter->current->prev != NULL) {
        iter->index--;
        iter->current = iter->current->prev;
    }
    return curr;
}

bool TacFIterDone(TacIterator *iter) {
    if (iter->current == NULL) return true;
    return (iter->current == NULL);
}

bool TacFIterFirst(TacIterator *iter) {
    return (iter->current->prev == NULL);
}

void printTACList(TacList *list) {
    TacIterator iter = createTacForwardIterator(list);
    while (!TacFIterDone(&iter)) {
        Tac *t = TacFIterNext(&iter);
        printf("(");

#define PTAC(A)     \
    case A:         \
        printf(#A); \
        break;

        switch (t->op) {
            PTAC(TAC_SYMBOL)
            PTAC(TAC_ADD)
        }
        if (t->dst != NULL) {
            printf("  dst: %s", t->dst ? t->dst->value : "0");
        }
        for (int i = 0; i < TAC_SRC_NUMBER; i++) {
            printf("  src %d: %s", i, t->src[i] ? t->src[i]->value : "0");
        }
        printf(")\n");
    }
}

void printCode(TacList *list) {
    TacIterator iter = createTacForwardIterator(list);
    while (!TacFIterDone(&iter)) {
        Tac *t = TacFIterNext(&iter);
        if (t->op == TAC_SYMBOL) {
            continue;
        }
        printf("(");

#define PTAC(A)     \
    case A:         \
        printf(#A); \
        break;

        switch (t->op) {
            case TAC_SYMBOL:
                break;

                PTAC(TAC_ADD)
        }
        printf("  dst: %s", t->dst ? t->dst->value : "0");
        for (int i = 0; i < TAC_SRC_NUMBER; i++) {
            printf("  src%X : %s", i + 10, t->src[i] ? t->src[i]->value : "0");
        }
        printf(")\n");
    }
}