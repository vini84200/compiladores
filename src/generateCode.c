#include "generateCode.h"
#include "ast.h"
#include "errorHandler.h"
#include "hash.h"
#include "lib.h"
#include "symbols.h"
#include "tac.h"
#include <stdio.h>

HashEntry *getListDst(TacList *list) {
    if (list == NULL) {
        fprintf(stderr, "WARN: no list in getListDst()");
        return 0;
    }
    if (list->last == NULL) {
        fprintf(stderr, "WARN: no last in get List Dst");
        return 0;
    }
    return list->last->tac.dst;
}

TacList *generateCodeExpr(TacList *code, AST_Type type, TacList *childLists[]) {
    switch (type) {
        case AST_EXPR_LIST:
            break;
        case AST_EXPR_ADD:
            appendTacList(code, createTac(
                                        TAC_ADD,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));

            break;
        case AST_EXPR_SUB:
            appendTacList(code, createTac(
                                        TAC_SUB,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_MUL:
            appendTacList(code, createTac(
                                        TAC_MUL,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_DIV:
            appendTacList(code, createTac(
                                        TAC_DIV,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_LESS:
            appendTacList(code, createTac(
                                        TAC_LT,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_GREATER:
            appendTacList(code, createTac(
                                        TAC_GT,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_LE:
            appendTacList(code, createTac(
                                        TAC_LEQ,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_GE:
            appendTacList(code, createTac(
                                        TAC_GEQ,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_EQ:
            appendTacList(code, createTac(
                                        TAC_EQ,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_NE:
            appendTacList(code, createTac(
                                        TAC_NEQ,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_AND:
            appendTacList(code, createTac(
                                        TAC_AND,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_OR:
            appendTacList(code, createTac(
                                        TAC_OR,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_EXPR_NOT:
            appendTacList(code, createTac(
                                        TAC_NOT,
                                        makeTemp(),
                                        getListDst(childLists[0]),
                                        NULL));
            break;
        case AST_EXPR_MINUS: {
            HashEntry *zero = hashInsert(getSymbolTable(), SYMBOL_LIT_INT, "0");
            appendTacList(code, createSymbolTac(zero));
            appendTacList(code, createTac(TAC_SUB, makeTemp(), zero, getListDst(childLists[0])));
        } break;

        case AST_EXPR_ARRAY_GET:

        case AST_EXPR_FUNC_CALL:
        case AST_EXPR_READ:
            appendTacList(code, createTac(
                                        TAC_READ,
                                        makeTemp(),
                                        NULL,
                                        NULL));
            break;

        default:
            printf("WARN: Unknown Expr Types");
    }
    return code;
}

TacList *generateCodeCmds(TacList *code, AST_Type type, TacList *childLists[]) {
    switch (type) {
        case AST_COMMAND_RETURN:
            appendTacList(code, createTac(
                                        TAC_RET,
                                        NULL,
                                        getListDst(childLists[0]),
                                        NULL));
            break;
        default:
            printf("WARN: Unknown Cmd Types");
    }
    return code;
}


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
        case AST_EXPR_ARRAY_GET:
        case AST_EXPR_FUNC_CALL:
        case AST_EXPR_LIST:
        case AST_EXPR_SUB:
        case AST_EXPR_MUL:
        case AST_EXPR_DIV:
        case AST_EXPR_LESS:
        case AST_EXPR_GREATER:
        case AST_EXPR_LE:
        case AST_EXPR_GE:
        case AST_EXPR_EQ:
        case AST_EXPR_NE:
        case AST_EXPR_AND:
        case AST_EXPR_OR:
        case AST_EXPR_NOT:
        case AST_EXPR_MINUS:
        case AST_EXPR_READ:
            generateCodeExpr(list, ast->type, childLists);
            break;
        case AST_EMPTY_COMMAND:
        case AST_COMMAND_BLOCK:
        case AST_COMMAND_LIST:
            // Nothing
            break;
        case AST_COMMAND_RETURN:
        case AST_COMMAND_ASSIGN:
        case AST_COMMAND_ASSIGN_ARRAY:
        case AST_COMMAND_PRINT_EXPR:
        case AST_COMMAND_PRINT_STRING:
        case AST_COMMAND_IF:
        case AST_COMMAND_WHILE:
            generateCodeCmds(list, ast->type, childLists);
            break;

        case AST_TYPE_INT:
        case AST_TYPE_FLOAT:
        case AST_TYPE_CHAR:
        case AST_VALUE:
            // Nothing
            break;
        case AST_PROGRAM:
        case AST_DECLARATION_LIST:
        case AST_CODE_LIST:
        case AST_IMPL_FUNC:
            // Nothing
            break;

        case AST_VAR_DECLARATION:
        case AST_FUNC_DECLARATION:
        case AST_ARRAY_DECLARATION:
        case AST_ARRAY_VALUES:
        case AST_PARAM_LIST:
        case AST_PARAM:
            // TODO
            break;
        default:
            criticalError("Unknown AST Type");
    }
    for (int i = 0; i < MAX_CHILDREN; i++) {
        list = joinTacList(childLists[i], list);
    }

    return list;
}
