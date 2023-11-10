//
// Created by vini84200 on 11/10/23.
//

#ifndef SCANNERDEFINITIONS_H
#define SCANNERDEFINITIONS_H

#include "lib.h"
#include "y.tab.h"
#include "hash.h"
#include "symbols.h"

void user_action() {
    incrementCollumn(yyleng);
}

#define YY_USER_ACTION user_action();

#endif //SCANNERDEFINITIONS_H
