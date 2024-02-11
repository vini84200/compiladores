//
// Created by vini84200 on 2/11/24.
//

#ifndef COMPILADORES_GRAPHBLOCK_H
#define COMPILADORES_GRAPHBLOCK_H

#include "tac.h"
#include <stdbool.h>
#include <stdlib.h>


typedef struct GraphBlock_t {
    int id;
    struct GraphBlock_t *predecessors;
    int nPredecessors;
    struct GraphBlock_t *cont;// next block
    struct GraphBlock_t *br;  // branch block
    bool hasBr;
    bool isReturn;
    bool isStart;

    HashEntry *function;
    HashEntry *startLabel;

    TacList *code;
} GraphBlock;

typedef struct LabelTable_t {
    HashEntry *label;
    GraphBlock *block;
    struct LabelTable_t *next;
} LabelTable;

GraphBlock *createGraphBlock(int id, TacList *code);

//** Create a graph for a function, the first block is the function entry point
// * @param function
// * @param code
// * @return the first block of the function
// */
GraphBlock *createFunctionGraph(HashEntry *function, TacList *code);

void addPredecessor(GraphBlock *block, GraphBlock *predecessor);
void appendLabelTable(LabelTable **table, HashEntry *label, GraphBlock *block);
LabelTable *createEmptyLabelTable();
LabelTable *getLabelTable(LabelTable *table, HashEntry *label);

void printGraphBlock(GraphBlock *block);
void printGraph(GraphBlock *block);

bool checkGraphReturns(GraphBlock *block);

#endif//COMPILADORES_GRAPHBLOCK_H
