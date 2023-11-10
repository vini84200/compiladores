//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#ifndef LIB_HEADER
#define LIB_HEADER

#include "hash.h"

int getLineNumber(void);
int isRunning(void);
void initSymbolTable(void);

HashTable *getSymbolTable();

extern int yylineno;
extern int running;

#endif// !LIB_HEADER
