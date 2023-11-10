//
// Created by vini84200 on 11/9/23.
//

#ifndef SYMBOLS_H
#define SYMBOLS_H

#define SYMBOL_LIT_INT 1
#define SYMBOL_LIT_FLOAT 2
#define SYMBOL_LIT_CHAR 3
#define SYMBOL_LIT_STRING 4
#define SYMBOL_IDENTIFIER 5

typedef int SymbolType;

char *getSymbolName(SymbolType type);

#endif //SYMBOLS_H
