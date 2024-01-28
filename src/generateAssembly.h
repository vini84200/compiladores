//
// Created by vini84200 on 1/27/24.
//

#ifndef COMPILADORES_GENERATEASSEMBLY_H
#define COMPILADORES_GENERATEASSEMBLY_H

#include "ast.h"
#include "tac.h"
void generateAssembly(TacList *code, AST *ast, FILE *outputFile);

#endif//COMPILADORES_GENERATEASSEMBLY_H
