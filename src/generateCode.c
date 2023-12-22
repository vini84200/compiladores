#include "generateCode.h"
#include "ast.h"
#include "hash.h"
#include "tac.h"
#include <stdio.h>


TacList *generateCode(AST *ast) {
    if (ast == NULL) return createTacList();
    TacList *list = createTacList();
    TacList *childLists[MAX_CHILDREN];

    for (int i = 0; i < MAX_CHILDREN; i++) {
        childLists[i] = generateCode(ast->children[i]);
    }
    switch (ast->type) {
        case AST_EXPR_IDENTIFIER:
        case AST_EXPR_LIT_INT:
        case AST_EXPR_LIT_CHAR:
        case AST_EXPR_LIT_FLOAT:
            appendTacList(list, createSymbolTac(ast->symbol));
            break;
        case AST_EXPR_ADD:
            // TODO: Check operands and identify the correct operation
            appendTacList(list, createTac(TAC_ADD, makeTemp(), childLists[0] ? childLists[0]->last ? childLists[0]->last->tac.dst : NULL : NULL, childLists[1] ? childLists[1]->last ? childLists[1]->last->tac.dst : NULL : NULL));
            break;
    }
    for (int i = 0; i < MAX_CHILDREN; i++) {
        list = joinTacList(childLists[i], list);
    }

    return list;
}
