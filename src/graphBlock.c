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
    gb->function = NULL;
    gb->startLabel = NULL;
    gb->predecessors = NULL;
    gb->nPredecessors = 0;
    gb->cont = NULL;
    gb->br = NULL;
    gb->hasBr = false;
    gb->isReturn = false;
    return gb;
}
GraphBlock *createFunctionGraph(HashEntry *function, TacList *code) {
    LabelTable *labelTable = createEmptyLabelTable();
    TacIterator it = createTacForwardIterator(code);
    GraphBlock *curr = createGraphBlock(0, createTacList());
    GraphBlock *firstBlock = curr;
    while (!TacFIterDone(&it)) {
        Tac *t = TacFIterNext(&it);
        if (t->op == TAC_LABEL) {
            // This finishes the current block, and starts a new one
            // saving the new block in the label table
            HashEntry *label = t->src[0];
            DEBUG("Creating block for label %s", label->value);
            if (curr->code->last != NULL) {
                // Check if the label is already in the table
                LabelTable *lt = getLabelTable(labelTable, label);
                if (lt == NULL) {
                    // Not in the table, add it
                    DEBUG("Label %s not in table", label->value);
                    curr->cont = createGraphBlock(curr->id + 1, createTacList());
                    addPredecessor(curr->cont, curr);
                    curr = curr->cont;
                    appendLabelTable(&labelTable, label, curr);
                } else {
                    // Already in the table, use it
                    DEBUG("Label %s in table", label->value);
                    curr->cont = lt->block;
                    lt->block->id = curr->id + 1;
                    addPredecessor(curr->cont, curr);
                    curr = curr->cont;
                }
            } else {
                // The last block was empty, just change the label
                DEBUG("The last block was empty, just change the label");
                curr->startLabel = label;
                // Check if the label is already in the table
                if (getLabelTable(labelTable, label) == NULL) {
                    appendLabelTable(&labelTable, label, curr);
                } else {
                    if (getLabelTable(labelTable, label)->block != curr) {
                        // The label was defined twice with different blocks, this is an error
                        CRITICAL("Label %s was defined twice", label->value);
                    }
                }
            }
            curr->startLabel = label;
            appendTacList(curr->code, *t);
        } else if (t->op == TAC_JUMP) {
            appendTacList(curr->code, *t);
            // This finishes the current block, and starts a new one
            // If the label is in the table, use it, otherwise create a empty in the table
            HashEntry *label = t->src[0];
            DEBUG("Jump to %s", label->value);
            LabelTable *lt = getLabelTable(labelTable, label);
            if (lt == NULL) {
                DEBUG("Label %s not in table", label->value);
                // Not in the table, add it
                curr->cont = createGraphBlock(curr->id + 1, createTacList());
                appendLabelTable(&labelTable, label, curr->cont);
            } else {
                // Already in the table, use it
                DEBUG("Label %s in table", label->value);
                curr->cont = lt->block;
            }
            addPredecessor(curr->cont, curr);

        } else if (t->op == TAC_IFZ) {
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
            cont = createGraphBlock(curr->id + 2, createTacList());
            curr->cont = cont;
            addPredecessor(cont, curr);

            curr->hasBr = true;
            curr = cont;

        } else {
            if (t->op == TAC_RET) {
                curr->isReturn = true;
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
void printGraph(GraphBlock *block) {
    GraphBlock *visited[1000];
    int nVisited = 0;
    GraphBlock *toVisit[1000];
    int nToVisit = 0;
    toVisit[nToVisit++] = block;
    while (nToVisit > 0) {
        GraphBlock *curr = toVisit[--nToVisit];
        visited[nVisited++] = curr;
        printGraphBlock(curr);
        if (curr->cont != NULL) {
            bool found = false;
            for (int i = 0; i < nVisited; i++) {
                if (visited[i] == curr->cont) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                toVisit[nToVisit++] = curr->cont;
            }
        }
        if (curr->br != NULL) {
            bool found = false;
            for (int i = 0; i < nVisited; i++) {
                if (visited[i] == curr->br) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                toVisit[nToVisit++] = curr->br;
            }
        }
    }
}
void printGraphBlock(GraphBlock *block) {
    printf("Block %d\n", block->id);
    printf("\n");
    printf("Code:\n");
    printCode(block->code, stdout);
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
    return block &&
        (block->isReturn ||
            ( block->cont && checkGraphReturns(block->cont) && (block->hasBr ? checkGraphReturns(block->br) : true)));
}
