//
// Created by vini84200 on 2/13/24.
//

#ifndef COMPILADORES_CALLSGRAPH_H
#define COMPILADORES_CALLSGRAPH_H

#include "graphBlock.h"
#include "hash.h"
/// CallsGraph is a graph that represents the calls between functions
/// The graph is represented by a list of nodes, each node has a list of calls
/// to other functions
/// The graph is directed, and the edges are the calls between functions
typedef struct CallsGraphNode {
    HashEntry *function;
    GraphBlock *startBlock;
    struct CallsGraphNode **calls;
    int nCalls;
    int callsSize;

    /// The directVarUsage is a list of the variables used in the function
    VarUsageList *directVarUsage;
    /// The completeVarUsage is a list of the variables used in the function and the functions that it calls
    VarUsageList *completeVarUsage;
} CallsGraphNode;

/// Create a new CallsGraph node
/// @param function the function that the node represents
/// @return the new node
CallsGraphNode *createCallsGraphNode(HashEntry *function);


/// The CallsGraph is a list of nodes, each node represents a function
typedef struct CallsGraph_t {
    CallsGraphNode **nodes;
    int nNodes;
    int nodesSize;
    bool populated;
} CallsGraph;

/// Create a new empty CallsGraph
/// @return the new CallsGraph
CallsGraph *createCallsGraph();

/// Add a function to the CallsGraph
/// @param graph the graph to add the function
/// @param function the function to add
void addFunction(CallsGraph *graph, HashEntry *function, GraphBlock *startBlock);

/// Add a call to a function to the node
/// @param graph the graph to add the call
/// @param node the node to add the call
/// @param function the function to call
/// @warning the function must be in the graph, otherwise the program will exit
void addCall(CallsGraph *graph, CallsGraphNode *node, HashEntry *function);

/// Populate the CallsGraph with the calls between functions
/// @param graph the graph to populate
/// The graph is populated by iterating over the nodes and the code of the functions
/// and adding the calls to the graph
/// @warning the graph must already have the functions added, otherwise the program will exit
void populateCallsGraph(CallsGraph *graph);

/// Print the CallsGraph in DOT format
/// @param graph the graph to print
void printCallsGraph(CallsGraph *graph);

/// Get the node that represents a function
/// @param graph the graph to search
/// @param function the function to search
/// @return the node that represents the function
CallsGraphNode *getFunctionNode(CallsGraph *graph, HashEntry *function);

/// Iterator for the CallsGraph
typedef struct CallsGraphIterator_t {
    CallsGraph *graph;
    int index;
} CallsGraphIterator;

/// Create a new iterator for the CallsGraph
/// @param graph the graph to iterate
/// @return the new iterator
CallsGraphIterator *createCallsGraphIterator(CallsGraph *graph);

/// Get the next node in the CallsGraph
/// @param it the iterator
/// @return the next node
CallsGraphNode *nextCallsGraphNode(CallsGraphIterator *it);

/// Check if there is a next node in the CallsGraph
/// @param it the iterator
/// @return true if there is a next node, false otherwise
bool hasNextCallsGraphNode(CallsGraphIterator *it);

/// Free the CallsGraph iterator
void freeCallsGraphIterator(CallsGraphIterator *it);

/// Get the dependencies of a function, that is, the functions that the function calls and the functions that they call, and so on
/// @param graph the graph to search
/// @param function the function to search
/// @param nDependencies the pointer to the output number of dependencies
/// @return the list of functions that the function calls, terminated by a NULL
HashEntry **getDependencies(CallsGraph *graph, HashEntry *function, int *nDependencies);


#endif//COMPILADORES_CALLSGRAPH_H
