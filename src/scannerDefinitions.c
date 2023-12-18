//
// Created by vini84200 on 11/10/23.
//

#ifndef SCANNERDEFINITIONS_H
#define SCANNERDEFINITIONS_H

#include "hash.h"
#include "ast.h"
#include "symbols.h"
#include "../generated/y.tab.h"
#include <stdio.h>


void user_action();
void resetCollumn();
HashTable *getSymbolTable();


#define YY_USER_ACTION user_action();

// int fileno(FILE *stream);

#endif//SCANNERDEFINITIONS_H
