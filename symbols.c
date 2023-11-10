//
// Created by vini84200 on 11/9/23.
//

#include "symbols.h"


char *getSymbolName(SymbolType type) {
    switch (type) {
        case SYMBOL_LIT_INT:
            return "int";
        case SYMBOL_LIT_FLOAT:
            return "float";
        case SYMBOL_LIT_CHAR:
            return "char";
        case SYMBOL_LIT_STRING:
            return "string";
        case SYMBOL_IDENTIFIER:
            return "identifier";
        default:
            return "unknown";
    }
}