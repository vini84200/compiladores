//
// Created by vini84200 on 2/11/24.
//

#include "funcCode.h"
FunctionCode *createFunctionCode(HashEntry *function, TacList *code) {
    FunctionCode *fc = (FunctionCode *) calloc(1, sizeof(FunctionCode));
    fc->function = function;
    fc->code = code;
    return fc;
}
FunctionCode *generateFunctionCode(HashEntry *function, AST *node) {
    TacList *code = generateCode(node);
    FunctionCode * fc = createFunctionCode(function, code);
    GraphBlock *graphStart = createFunctionGraph(function, code);
    fc->graphStart = graphStart;
    return fc;
}
