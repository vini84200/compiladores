//
// Created by vini84200 on 2/11/24.
//

#include "graphBlock.h"
#include "logs.h"
#include "tac.h"
GraphBlock *createGraphBlock(int id, TacList *code) {
    GraphBlock *gb = (GraphBlock *) calloc(1, sizeof(GraphBlock));
    gb->id = id;
    gb->code = code;
    gb->calledFunction = NULL;
    gb->startLabel = NULL;
    gb->predecessors = NULL;
    gb->nPredecessors = 0;
    gb->cont = NULL;
    gb->br = NULL;
    gb->hasBr = false;
    gb->isReturn = false;
    gb->isStart = false;
    gb->isCall = false;
    return gb;
}
GraphBlock *createFunctionGraph(HashEntry *function, TacList *code) {
    LabelTable *labelTable = createEmptyLabelTable();
    TacIterator it = createTacForwardIterator(code);
    GraphBlock *curr = createGraphBlock(0, createTacList());
    curr->isStart = true;
    int id = 0;
    GraphBlock *firstBlock = curr;
    while (!TacFIterDone(&it)) {
        Tac *t = TacFIterNext(&it);
        if (t->op == TAC_LABEL) {
            // This finishes the current block, and starts a new one
            // saving the new block in the label table
            HashEntry *label = t->src[0];
            DEBUG("Creating block for label %s", label->value);
            // Check if the label is already in the table
            LabelTable *lt = getLabelTable(labelTable, label);
            if (lt == NULL) {
                // Not in the table, add it
                DEBUG("Label %s not in table", label->value);
                GraphBlock *newBlock = createGraphBlock(++id, createTacList());
                if (curr != NULL) {
                    curr->cont = newBlock;
                    addPredecessor(newBlock, curr);
                }
                curr = newBlock;
                appendLabelTable(&labelTable, label, curr);
            } else {
                // Already in the table, use it
                DEBUG("Label %s in table", label->value);
                if (curr != NULL) {
                    curr->cont = lt->block;
                    addPredecessor(lt->block, curr);
                }
                if (lt->block->id != -1) {
                    CRITICAL("Label %s already in table, but the id is not the expected", label->value);
                }
                lt->block->id = ++id;
                curr = lt->block;
            }
            curr->startLabel = label;
            appendTacList(curr->code, *t);
        } else if (t->op == TAC_JUMP) {
            if (curr == NULL) {
                WARN("Jump without a previous label, this is dead code");
                continue;
            }
            appendTacList(curr->code, *t);
            // This finishes the current block, and starts a new one
            // If the label is in the table, use it, otherwise create a empty in the table
            HashEntry *label = t->src[0];
            DEBUG("Jump to %s", label->value);
            LabelTable *lt = getLabelTable(labelTable, label);
            if (lt == NULL) {
                DEBUG("Label %s not in table", label->value);
                // Not in the table, add it
                curr->cont = createGraphBlock(-1, createTacList());
                appendLabelTable(&labelTable, label, curr->cont);
            } else {
                // Already in the table, use it
                DEBUG("Label %s in table", label->value);
                curr->cont = lt->block;
            }
            addPredecessor(curr->cont, curr);
            curr = NULL;
        } else if (t->op == TAC_IFZ) {
            if (curr == NULL) {
                WARN("IfZ without a previous jump or label, this is dead code");
                continue;
            }
            appendTacList(curr->code, *t);
            // This ends the current block, and starts two new ones
            // If the label is in the table, use it, otherwise create a empty in the table
            DEBUG("If Z");
            HashEntry *label = t->src[1];
            LabelTable *lt = getLabelTable(labelTable, label);
            GraphBlock *br = NULL;
            GraphBlock *cont = NULL;
            // Create the block for the branch
            if (lt == NULL) {
                // Not in the table, add it
                DEBUG("Branch Label %s not in table", label->value);
                br = createGraphBlock(-1, createTacList());
                appendLabelTable(&labelTable, label, br);
            } else {
                // Already in the table, use it
                DEBUG("Branch Label %s in table", label->value);
                br = lt->block;
            }
            addPredecessor(br, curr);
            curr->br = br;
            // Create the block for the continue
            //  block is the next block in the code
            DEBUG("Creating continue block");
            cont = createGraphBlock(++id, createTacList());
            curr->cont = cont;
            addPredecessor(cont, curr);

            curr->hasBr = true;
            curr = cont;

        } else if (t->op == TAC_RET) {
            if (curr == NULL) {
                WARN("Return without a previous jump or label, this is dead code");
                continue;
            }
            // This ends the current block, and starts a new one
            // saving the new block in the label table
            DEBUG("Return");
            curr->isReturn = true;
            appendTacList(curr->code, *t);
            // The return block is the last block and does not have a continue
            curr->cont = NULL;
            curr = NULL;
        } else if (t->op == TAC_CALL) {
            if (curr == NULL) {
                WARN("Call without a previous jump or label, this is dead code");
                continue;
            }
            // Calls should be their own block, as they can read/write multiple global variables
            // This ends the current block, and starts a new one
            curr->cont = createGraphBlock(++id, createTacList());
            addPredecessor(curr->cont, curr);
            curr = curr->cont;
            curr->isCall = true;
            curr->calledFunction = t->src[0];
            appendTacList(curr->code, *t);
            // Then the next block should be the continuation of the call
            curr->cont = createGraphBlock(++id, createTacList());
            addPredecessor(curr->cont, curr);
            curr = curr->cont;

        } else {
            if (curr == NULL) {
                WARN("Instruction without a previous jump or label, this is dead code");
                continue;
            }
            // Add the instruction to the current block
            appendTacList(curr->code, *t);
        }
    }
    return firstBlock;
}
LabelTable *createEmptyLabelTable() {
    return NULL;
}

#define X(ID) [ID] = #ID
#define COMMA ,
static const char *TacToString[TAC_MAX] = {
        TAC_OPT_Gen(X, COMMA)};

const char *getTacOpString(TacOp op) {
    return TacToString[op];
}

char *escapeString(char *str) {
    char *newStr = (char *) calloc(1, 1024);
    int j = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '"') {
            newStr[j++] = '\\';
        }
        newStr[j++] = str[i];
    }
    return newStr;
}

char *getTacString(Tac *t) {
    char *str = (char *) calloc(1, 1024);
    if (t->dst != NULL) {
        sprintf(str, "%s <- %s %s %s", escapeString(t->dst->value), getTacOpString(t->op), t->src[0] == NULL ? "" : escapeString(t->src[0]->value), t->src[1] == NULL ? "" : escapeString(t->src[1]->value));
    }
    else {
        sprintf(str, "%s %s %s", getTacOpString(t->op), t->src[0] == NULL ? "" : escapeString(t->src[0]->value), t->src[1] == NULL ? "" : escapeString(t->src[1]->value));
    }
    return str;
}

void printControlFlowGraph(GraphBlock *block, HashEntry *function) {
    GraphBlockIterator *it2 = createGraphBlockIterator(block);
    char *functionName = function->value;
    while (hasNextGraphBlock(it2)) {
        GraphBlock *curr = nextGraphBlock(it2);
//        if (curr->isCall) {
//            printf("%s_%d [shape=parallelogram, label=\"Call %s\"]\n", functionName, curr->id, curr->calledFunction->value);
//        } else {
            char *shape = curr->isReturn ? "invtriangle" : curr->isCall ? "parallelogram" : curr->hasBr ? "diamond" : "box";
           printf("%s_%d [shape=%s, label=\"", functionName, curr->id, shape);
            TacIterator it = createTacForwardIterator(curr->code);
            while (!TacFIterDone(&it)) {
                Tac *t = TacFIterNext(&it);
                if (t->op == TAC_SYMBOL) {
                    continue;
                }
                char *str = getTacString(t);
                printf("%s\\n", str);
                free(str);
            }
            printf("\"]\n");
//        }

        if (curr->br != NULL) {
            printf("%s_%d -> %s_%d [label=\"IfZ\"]\n", functionName, curr->id, functionName, curr->br->id);
        }
        if (curr->cont != NULL) {
            printf("%s_%d -> %s_%d [label=\"Cont\"]\n", functionName, curr->id, functionName, curr->cont->id);
        }
        //        DEBUG("VISITING %d", curr->id)
    }
    freeGraphBlockIterator(it2);
}

void printGraphBlock(GraphBlock *block) {
    printf("Block %d\n", block->id);
    printf("\n");
    //    printf("Code:\n");
    //    printCode(block->code, stdout);
    printf("\n");
    if (block->isReturn) {
        printf("Return\n");
    }
    if (block->hasBr) {
        printf("If Z Branch to %d\n", block->br->id);
    }
    if (block->cont != NULL) {
        printf("Continue to %d\n", block->cont->id);
    } else {
        printf("End of function\n");
    }
}
LabelTable *getLabelTable(LabelTable *table, HashEntry *label) {
    LabelTable *curr = table;
    while (curr != NULL) {
        if (curr->label == label) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}
void addPredecessor(GraphBlock *block, GraphBlock *predecessor) {
    GraphBlock *newP = realloc(block->predecessors, (block->nPredecessors + 1) * sizeof(GraphBlock));
    if (newP == NULL) {
        CRITICAL("Could not allocate memory for predecessors");
    }
    block->predecessors = newP;
    block->predecessors[block->nPredecessors++] = *predecessor;
}
void appendLabelTable(LabelTable **table, HashEntry *label, GraphBlock *block) {
    LabelTable *curr = *table;
    if (curr == NULL) {
        *table = (LabelTable *) calloc(1, sizeof(LabelTable));
        (*table)->label = label;
        (*table)->block = block;
        return;
    }
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = (LabelTable *) calloc(1, sizeof(LabelTable));
    curr->next->label = label;
    curr->next->block = block;
}
bool checkGraphReturns(GraphBlock *block) {
    GraphBlockIterator *it = createGraphBlockIterator(block);
    while (hasNextGraphBlock(it)) {
        GraphBlock *curr = nextGraphBlock(it);
        // If the block is not a return and does not have a continue, the function does not return
        if (curr->cont == NULL && !curr->isReturn) {
            freeGraphBlockIterator(it);
            return false;
        }
    }
    freeGraphBlockIterator(it);
    return true;
}
GraphBlockIterator *createGraphBlockIterator(GraphBlock *block) {
    GraphBlockIterator *it = (GraphBlockIterator *) calloc(1, sizeof(GraphBlockIterator));
    it->block = block;
    it->toVisit = (GraphBlock **) calloc(8, sizeof(GraphBlock *));
    it->toVisit[0] = block;
    it->nToVisit = 1;
    it->toVisitSize = 8;
    it->visited = (GraphBlock **) calloc(8, sizeof(GraphBlock *));
    it->visitedSize = 8;
    it->nVisited = 0;
    return it;
}
GraphBlock *nextGraphBlock(GraphBlockIterator *it) {
    if (it->nToVisit == 0) {
        return NULL;
    }
    GraphBlock *curr = it->toVisit[--it->nToVisit];
    if (it->nVisited == it->visitedSize) {
        it->visitedSize *= 2;
        GraphBlock **new_visited = realloc(it->visited, it->visitedSize * sizeof(GraphBlock *));
        if (new_visited == NULL) {
            CRITICAL("Could not allocate memory for visited");
        }
        it->visited = new_visited;
    }
    it->visited[it->nVisited++] = curr;
    if (curr->br != NULL && !curr->isReturn) {
        bool found = false;
        for (int i = 0; i < it->nVisited; i++) {
            if (it->visited[i] == curr->br) {
                found = true;
                break;
            }
        }
        for (int i = 0; i < it->nToVisit; i++) {
            if (it->toVisit[i] == curr->br) {
                found = true;
                break;
            }
        }
        if (!found) {
            if (it->nToVisit == it->toVisitSize) {
                it->toVisitSize *= 2;
                GraphBlock **newtoVisit = realloc(it->toVisit, it->toVisitSize * sizeof(GraphBlock *));
                if (newtoVisit == NULL) {
                    CRITICAL("Could not allocate memory for toVisit");
                }
                it->toVisit = newtoVisit;
            }
            it->toVisit[it->nToVisit++] = curr->br;
        }
    }
    if (curr->cont != NULL && !curr->isReturn) {
        bool found = false;
        for (int i = 0; i < it->nVisited; i++) {
            if (it->visited[i] == curr->cont) {
                found = true;
                break;
            }
        }
        for (int i = 0; i < it->nToVisit; i++) {
            if (it->toVisit[i] == curr->cont) {
                found = true;
                break;
            }
        }
        if (!found) {
            if (it->nToVisit == it->toVisitSize) {
                it->toVisitSize *= 2;
                GraphBlock **newtoVisit = realloc(it->toVisit, it->toVisitSize * sizeof(GraphBlock *));
                if (newtoVisit == NULL) {
                    CRITICAL("Could not allocate memory for toVisit");
                }
                it->toVisit = newtoVisit;
            }
            it->toVisit[it->nToVisit++] = curr->cont;
        }
    }
    it->block = curr;
    return curr;
}
bool hasNextGraphBlock(GraphBlockIterator *it) {
    return it->nToVisit > 0;
}
void freeGraphBlockIterator(GraphBlockIterator *it) {
    free(it->toVisit);
    free(it->visited);
    free(it);
}
VarUsage *createVarUsageGlobal(HashEntry *var, bool isRead, bool isWritten) {
    VarUsage *vu = (VarUsage *) calloc(1, sizeof(VarUsage));
    vu->var = var;
    vu->isRead = isRead;
    vu->isWritten = isWritten;
    vu->isGlobal = true;
    vu->isParam = false;
    return vu;
}
VarUsage *createVarUsageLocal(HashEntry *var, bool isRead, bool isWritten) {
    VarUsage *vu = (VarUsage *) calloc(1, sizeof(VarUsage));
    vu->var = var;
    vu->isRead = isRead;
    vu->isWritten = isWritten;
    vu->isGlobal = false;
    vu->isParam = false;
    return vu;
}
VarUsage *createVarUsageParam(HashEntry *var, bool isRead, bool isWritten) {
    VarUsage *vu = (VarUsage *) calloc(1, sizeof(VarUsage));
    vu->var = var;
    vu->isRead = isRead;
    vu->isWritten = isWritten;
    vu->isGlobal = false;
    vu->isParam = true;
    return vu;
}
VarUsageList *createVarUsageList() {
    VarUsageList *list = (VarUsageList *) calloc(1, sizeof(VarUsageList));
    list->nUsages = 0;
    list->usagesSize = 10;
    list->usages = (VarUsage **) calloc(list->usagesSize, sizeof(VarUsage *));
    return list;
}
void addVarUsage(VarUsageList *list, VarUsage *usage) {
    // Check if the usage is already in the list
    for (int i = 0; i < list->nUsages; i++) {
        if (list->usages[i]->var == usage->var) {
            // Just merge the usages
            list->usages[i]->isRead |= usage->isRead;
            list->usages[i]->isWritten |= usage->isWritten;
            return;
        }
    }
    if (list->nUsages == list->usagesSize) {
        list->usagesSize *= 2;
        VarUsage **newUsages = realloc(list->usages, list->usagesSize * sizeof(VarUsage *));
        if (newUsages == NULL) {
            CRITICAL("Could not allocate memory for usages");
        }
        list->usages = newUsages;
    }
    list->usages[list->nUsages++] = usage;
}
void printVarUsageList(VarUsageList *list) {
    VarUsageIterator *it = createVarUsageIterator(list);
    VarUsage *vu;
    while ((vu = nextVarUsage(it)) != NULL) {
        char *type = vu->isGlobal ? "Global" : vu->isParam ? "Param" : "Local";
        printf("%s (%s) %s %s\n", vu->var->value, type, vu->isRead ? "Read" : "", vu->isWritten ? "Written" : "");
    }
    freeVarUsageIterator(it);
}
void freeVarUsageList(VarUsageList *list) {
    for (int i = 0; i < list->nUsages; i++) {
        free(list->usages[i]);
    }
    free(list->usages);
    free(list);
}
VarUsageIterator *createVarUsageIterator(VarUsageList *list) {
    VarUsageIterator *it = (VarUsageIterator *) calloc(1, sizeof(VarUsageIterator));
    it->list = list;
    it->index = 0;
    it->filteroutGlobal = false;
    it->filteroutLocal = false;
    it->filteroutParam = false;
    it->filteroutRead = false;
    it->filteroutWritten = false;
    return it;
}
VarUsage *nextVarUsage(VarUsageIterator *it) {
    while (it->index < it->list->nUsages) {
        VarUsage *vu = it->list->usages[it->index];
        if (
                (it->filteroutRead && vu->isRead) ||
                (it->filteroutWritten && vu->isWritten) ||
                (it->filteroutGlobal && vu->isGlobal) ||
                (it->filteroutLocal && !vu->isGlobal && !vu->isParam) ||
                (it->filteroutParam && vu->isParam)
                ) {
            it->index++;
        } else {
            it->index++;
            return vu;
        }
    }
    return NULL;
}
bool hasNextVarUsage(VarUsageIterator *it) {
    return it->index < it->list->nUsages;
}
void freeVarUsageIterator(VarUsageIterator *it) {
    free(it);
}
VarUsage *getVarUsage(VarUsageList *list, HashEntry *var) {
    for (int i = 0; i < list->nUsages; i++) {
        if (list->usages[i]->var == var) {
            return list->usages[i];
        }
    }
    return NULL;
}
void setVarUsage(VarUsageList *list, HashEntry *var, HashEntry *function, bool isRead, bool isWritten) {
    VarUsage *vu = getVarUsage(list, var);
    if (vu == NULL) {
        if (var->type == SYMBOL_IDENTIFIER) {
            if (var->identifier->type->plural==TYPE_NATURE_FUNCTION) {
                // Function calls are not considered as read or written, because they are immutable
                // So we just ignore them, as they are not relevant for the analysis
                // The function reads and writes after the call are checked in another function
                return;
            }
            if (var->identifier->bound->bound_type == BOUND_TYPE_GLOBAL) {
                vu = createVarUsageGlobal(var, false, false);
            } else if (var->identifier->bound->bound_type == BOUND_TYPE_PARAM) {
                vu = createVarUsageParam(var, false, false);
            } else {
                CRITICAL("Unknown bound type");
                exit(1);
            }
            addVarUsage(list, vu);
        } else if (var->type == SYMBOL_TEMP_ID) {
            vu = createVarUsageLocal(var, false, false);
            addVarUsage(list, vu);
        }
    }
    if (vu != NULL) {
        vu->isRead |= isRead;
        vu->isWritten |= isWritten;
    }
}
VarUsageList *getFunctionDirectVarUsage(GraphBlock *block, HashEntry *function) {
    GraphBlockIterator *it = createGraphBlockIterator(block);
    VarUsageList *list = createVarUsageList();

    while (hasNextGraphBlock(it)) {
        GraphBlock *curr = nextGraphBlock(it);
        TacIterator it2 = createTacForwardIterator(curr->code);
        while (!TacFIterDone(&it2)) {
            Tac *t = TacFIterNext(&it2);
            if (t->src[0] != NULL) {
                setVarUsage(list, t->src[0], function, true, false);
            }
            if (t->src[1] != NULL) {
                setVarUsage(list, t->src[1], function, true, false);
            }
            if (t->dst != NULL) {
                setVarUsage(list, t->dst, function, false, true);
            }
        }
    }

    return list;
}
