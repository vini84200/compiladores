//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "lib.h"
#include "y.tab.h"
#include "lex.yy.h"


#include <stdio.h>

int lineNumber = 1;
int running = 1;
int end_collumn = 0;
HashTable symbolTable;
AST *ast = NULL;

int isRunning() { return running; }

int getLineNumber() { return yylineno; }
int getCollumn(void) {
    return end_collumn - yyleng + 1;
}

void initSymbolTable() { hashCreate(&symbolTable, 997); }

HashTable *getSymbolTable() { return &symbolTable; }
AST *getAST() {
    return ast;
}
AST *setAST(AST *newAst) {
    AST *oldAst = ast;
    ast = newAst;
    return oldAst;
}
void resetCollumn(void) {
    end_collumn = 0;
}
void incrementCollumn(int increment) {

    end_collumn += increment;
}
void user_action() {
    incrementCollumn(yyleng);
    yylloc.first_line = yylloc.last_line = getLineNumber();
    yylloc.first_column = getCollumn();
    yylloc.last_column = getCollumn() + yyleng - 1;


#ifdef DEBUG

#endif /* ifdef DEBUG
     */
}
