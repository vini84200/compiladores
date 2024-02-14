//
// Created by vini84200 on 2/13/24.
//

#include "callsGraph.h"
#include "logs.h"
CallsGraphNode *createCallsGraphNode(HashEntry *function) {
    CallsGraphNode *node = malloc(sizeof(CallsGraphNode));
    node->function = function;
    node->nCalls = 0;
    node->callsSize = 10;
    node->calls = malloc(sizeof(CallsGraphNode *) * node->callsSize);
    node->startBlock = NULL;
    node->directVarUsage = NULL;
    node->completeVarUsage = NULL;
    return node;
}
void addCall(CallsGraph *graph, CallsGraphNode *node, HashEntry *function) {
    CallsGraphNode *found = getFunctionNode(graph, function);
    if (found == NULL) {
        CRITICAL("Function not found in calls graph");
        exit(1);
    }
    if (node->nCalls == node->callsSize) {
        node->callsSize *= 2;
        CallsGraphNode **n = realloc(node->calls, sizeof(CallsGraphNode *) * node->callsSize);
        if (n == NULL) {
            CRITICAL("Failed to realloc calls array");
            exit(1);
        }
        node->calls = n;
    }
    node->calls[node->nCalls++] = found;
}
CallsGraph *createCallsGraph() {
    CallsGraph *graph = malloc(sizeof(CallsGraph));
    graph->nNodes = 0;
    graph->nodesSize = 10;
    graph->nodes = malloc(sizeof(CallsGraphNode *) * graph->nodesSize);
    graph->populated = false;
    return graph;
}
void addFunction(CallsGraph *graph, HashEntry *function, GraphBlock *startBlock) {
    if (graph->nNodes == graph->nodesSize) {
        graph->nodesSize *= 2;
        CallsGraphNode **n = realloc(graph->nodes, sizeof(CallsGraphNode *) * graph->nodesSize);
        if (n == NULL) {
            CRITICAL("Failed to realloc nodes array");
            exit(1);
        }
        graph->nodes = n;
    }
    CallsGraphNode *node = createCallsGraphNode(function);
    node->startBlock = startBlock;
    graph->nodes[graph->nNodes++] = node;
}
void populateCallsGraph(CallsGraph *graph) {
    if (graph->populated) {
        return;
    }
    graph->populated = true;
    for (int i = 0; i < graph->nNodes; i++) {
        CallsGraphNode *node = graph->nodes[i];
        GraphBlock *block = node->startBlock;
        GraphBlockIterator *it = createGraphBlockIterator(block);
        while (hasNextGraphBlock(it)) {
            GraphBlock *b = nextGraphBlock(it);
            if (b->isCall) {
                HashEntry *function = b->calledFunction;
                CallsGraphNode *found = getFunctionNode(graph, function);
                if (found == NULL) {
                    CRITICAL("Function not found in calls graph");
                    exit(1);
                }
                addCall(graph, node, function);
            }
        }
    }
}
void printCallsGraph(CallsGraph *graph) {
    printf("CallsGraph:\n");
    printf(" ---- DOT format ----\n");
    printf("digraph G {\n");
    for (int i = 0; i < graph->nNodes; i++) {
        // Prints each function control flow graph as a subgraph
        printf("  subgraph cluster_%d {\n", i);
        printf("    label = \"%s\";\n", graph->nodes[i]->function->value);
        printControlFlowGraph(graph->nodes[i]->startBlock, graph->nodes[i]->function);
        printf("  }\n");

        CallsGraphNode *node = graph->nodes[i];
        for (int j = 0; j < node->nCalls; j++) {
            CallsGraphNode *call = node->calls[j];
            printf("  \"%s\" -> \"%s\";\n", node->function->value, call->function->value);
        }
    }
    printf("}\n");
    printf(" ---- END ----\n");
}
CallsGraphNode *getFunctionNode(CallsGraph *graph, HashEntry *function) {
    for (int i = 0; i < graph->nNodes; i++) {
        CallsGraphNode *node = graph->nodes[i];
        if (node->function == function) {
            return node;
        }
    }
    return NULL;
}
CallsGraphIterator *createCallsGraphIterator(CallsGraph *graph) {
    CallsGraphIterator *it = malloc(sizeof(CallsGraphIterator));
    it->graph = graph;
    it->index = 0;
    return it;
}
CallsGraphNode *nextCallsGraphNode(CallsGraphIterator *it) {
    if (it->index == it->graph->nNodes) {
        return NULL;
    }
    return it->graph->nodes[it->index++];
}
bool hasNextCallsGraphNode(CallsGraphIterator *it) {
    return it->index < it->graph->nNodes;
}
void freeCallsGraphIterator(CallsGraphIterator *it) {
    free(it);
}
HashEntry **getDependencies(CallsGraph *graph, HashEntry *function, int *nDependencies) {
    HashEntry **dependencies = malloc(sizeof(HashEntry *) * graph->nNodes);
    int n = 0;
    // Add the direct dependencies
    CallsGraphNode *node = getFunctionNode(graph, function);
    for (int i = 0; i < node->nCalls; i++) {
        dependencies[n++] = node->calls[i]->function;
    }
    // For each dependency, add their dependencies
    for (int i = 0; i < n; i++) {
        CallsGraphNode *dep = getFunctionNode(graph, dependencies[i]);
        for (int j = 0; j < dep->nCalls; j++) {
            HashEntry *f = dep->calls[j]->function;
            bool found = false;
            for (int k = 0; k < n; k++) {
                if (dependencies[k] == f) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                dependencies[n++] = f;
            }
        }
    }
    *nDependencies = n;
    return dependencies;
}
