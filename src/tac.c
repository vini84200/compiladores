#include "tac.h"
#include "errorHandler.h"
#include "logs.h"
#include <stdio.h>
#include <stdlib.h>

#define X(ID) [ID] = #ID
#define COMMA ,
static const char *TacToString[TAC_MAX] = {
        TAC_OPT_Gen(X, COMMA)};

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
    TacListItem *item = tacList->head;
    while (item != NULL) {
        TacListItem *next = item->next;
        free(item);
        item = next;
    }

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
    return (iter->current == NULL);
}

bool TacFIterFirst(TacIterator *iter) {
    return (iter->current->prev == NULL);
}
#define EMPTY

void printTACList(TacList *list) {
    TacIterator iter = createTacForwardIterator(list);
    while (!TacFIterDone(&iter)) {
        Tac *t = TacFIterNext(&iter);
        printf("(%-14s dst:%-10s srcA:%-10s srcB:%-10s)\n",
               TacToString[t->op],
               t->dst ? t->dst->value : "0",
               t->src[0] ? t->src[0]->value : "0",
               t->src[1] ? t->src[1]->value : "0");
        if (t->op == TAC_ENDFUN)
            printf("\n");
    }
}

void printCode(TacList *list, FILE *fileOut) {
    INFO("Printing generated intermediate code");
    TacIterator iter = createTacForwardIterator(list);
    while (!TacFIterDone(&iter)) {
        Tac *t = TacFIterNext(&iter);
        if (t->op == TAC_SYMBOL) {
            continue;
        }
        if (t->op == TAC_LABEL) {
            if (!t->src[0]) ERROR("Invalid label");
            fprintf(fileOut, "%s:\n", t->src[0]->value);
            continue;
        }
        fprintf(fileOut, "(%-14s %-10s A:%-10s B:%-10s)\n",
                TacToString[t->op],
                t->dst ? t->dst->value : "0",
                t->src[0] ? t->src[0]->value : "0",
                t->src[1] ? t->src[1]->value : "0");
        if (t->op == TAC_ENDFUN)
            fprintf(fileOut, "\n");
    }
}
