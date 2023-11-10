//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "lib.h"


#include <stdio.h>

int lineNumber = 1;
int running = 1;
int end_collumn = 0;
HashTable symbolTable;

int isRunning() { return running; }

int getLineNumber() { return yylineno; }
int getCollumn(void) {
    return end_collumn - yyleng + 1;
}

void initSymbolTable() { hashCreate(&symbolTable, 997); }

HashTable *getSymbolTable() { return &symbolTable; }
void resetCollumn(void) {
    end_collumn = 0;
}
void incrementCollumn(int increment) {

    end_collumn += increment;
}