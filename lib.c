//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "lib.h"

int lineNumber = 1;
int running = 1;
HashTable symbolTable;

int isRunning() { return running; }

int getLineNumber() { return yylineno; }

void initSymbolTable() { hashCreate(&symbolTable, 997); }

HashTable *getSymbolTable() { return &symbolTable; }