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

    HashEntry *calledFunction;
    HashEntry *startLabel;

    TacList *code;
    bool isCall;
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
void printControlFlowGraph(GraphBlock *block, HashEntry *function);

bool checkGraphReturns(GraphBlock *block);

typedef struct GraphBlockIterator_t {
    GraphBlock **visited;
    GraphBlock **toVisit;
    int nToVisit;
    int nVisited;
    int toVisitSize;
    int visitedSize;
    GraphBlock *block;
} GraphBlockIterator;

GraphBlockIterator *createGraphBlockIterator(GraphBlock *block);
GraphBlock *nextGraphBlock(GraphBlockIterator *it);
bool hasNextGraphBlock(GraphBlockIterator *it);
void freeGraphBlockIterator(GraphBlockIterator *it);

typedef struct VarUsage_t {
    HashEntry *var;
    bool isRead;
    bool isWritten;
    bool isParam;
    bool isGlobal;
} VarUsage;

VarUsage *createVarUsageGlobal(HashEntry *var, bool isRead, bool isWritten);
VarUsage *createVarUsageLocal(HashEntry *var, bool isRead, bool isWritten);
VarUsage *createVarUsageParam(HashEntry *var, bool isRead, bool isWritten);

typedef struct VarUsageList_t {
    VarUsage **usages;
    int nUsages;
    int usagesSize;
} VarUsageList;

VarUsageList *createVarUsageList();
void addVarUsage(VarUsageList *list, VarUsage *usage);
void printVarUsageList(VarUsageList *list);

void freeVarUsageList(VarUsageList *list);

typedef struct VarUsageIterator_t {
    VarUsageList *list;
    int index;
    // Filters
    bool filteroutRead;
    bool filteroutWritten;
    bool filteroutGlobal;
    bool filteroutLocal;
    bool filteroutParam;
} VarUsageIterator;

VarUsageIterator *createVarUsageIterator(VarUsageList *list);
VarUsage *nextVarUsage(VarUsageIterator *it);
bool hasNextVarUsage(VarUsageIterator *it);
void freeVarUsageIterator(VarUsageIterator *it);

VarUsage *getVarUsage(VarUsageList *list, HashEntry *var);

void setVarUsage(VarUsageList *list, HashEntry *var, HashEntry *function, bool isRead, bool isWritten);

VarUsageList *getFunctionDirectVarUsage(GraphBlock *block, HashEntry *function);



#endif//COMPILADORES_GRAPHBLOCK_H
