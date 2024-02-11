#ifndef COMPILADORES_FUNCCODE_H
#define COMPILADORES_FUNCCODE_H

#include "ast.h"
#include "generateCode.h"
#include "graphBlock.h"
#include "tac.h"
#include <malloc.h>

typedef struct FunctionCode {
    HashEntry *function;
    TacList *code;
    GraphBlock *graphStart;
} FunctionCode;

FunctionCode *createFunctionCode(HashEntry *function, TacList *code);

FunctionCode *generateFunctionCode(HashEntry *function, AST *node);



#endif//COMPILADORES_FUNCCODE_H
