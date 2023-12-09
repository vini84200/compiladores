//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#ifndef LIB_HEADER
#define LIB_HEADER

#include "ast.h"
#include "hash.h"
#include "lex.yy.h"

int getLineNumber(void);
int getCollumn(void);
int isRunning(void);
void initSymbolTable(void);

HashTable *getSymbolTable();

/// @brief Returns the current AST
AST *getAST();

/// @brief Sets the AST and returns the old one
AST *setAST(AST *newAst);

void resetCollumn(void);
void incrementCollumn(int increment);


#endif// !LIB_HEADER
