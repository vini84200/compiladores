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
        WARN("no list in getListDst()");
        return 0;
    }
    if (list->last == NULL) {
        WARN("no last in get List Dst");
        return 0;
    }
    return list->last->tac.dst;
}

TacList *generateCodeExpr(TacList *code, AST *ast, TacList *childLists[]) {
    AST_Type type = ast->type;
    switch (type) {
        case AST_EXPR_LIST:
            appendTacList(code, createTac(
                                        TAC_ARG,
                                        NULL,
                                        getListDst(childLists[0]), NULL));
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
            appendTacList(code, createTac(
                                        TAC_LOAD_ARRAY,
                                        makeTemp(),
                                        ast->symbol,
                                        getListDst(childLists[0])));
            break;
        case AST_EXPR_FUNC_CALL:
            appendTacList(code, createTac(
                                        TAC_CALL,
                                        makeTemp(),
                                        ast->symbol,
                                        NULL));
            break;

        case AST_EXPR_READ:
            appendTacList(code, createTac(
                                        TAC_READ,
                                        makeTemp(),
                                        NULL,
                                        NULL));
            break;

        default:
            WARN("Unknown Expr Types");
    }
    return code;
}

TacList *generateCodeCmds(TacList *code, AST *ast, TacList *childLists[]) {
    AST_Type type = ast->type;
    switch (type) {
        case AST_COMMAND_RETURN:
            appendTacList(code, createTac(
                                        TAC_RET,
                                        NULL,
                                        getListDst(childLists[0]),
                                        NULL));
            break;
        case AST_COMMAND_ASSIGN:
            appendTacList(code, createTac(
                                        TAC_MOVE,
                                        ast->symbol,
                                        getListDst(childLists[0]),
                                        NULL));
            break;
        case AST_COMMAND_ASSIGN_ARRAY:
            appendTacList(code, createTac(
                                        TAC_MOVE_ARRAY,
                                        ast->symbol,
                                        getListDst(childLists[0]),
                                        getListDst(childLists[1])));
            break;
        case AST_COMMAND_IF: {
            // (AST_IF sym:NULL [0]: expr [1]: cmd then [2]: cmd else)
            // Generate:
            // r = [EXPR]
            // ifz r b
            // [THEN]
            // jmp end
            // b:
            // [ELSE]
            // end:

            TacList *expr = childLists[0];
            HashEntry *res = getListDst(expr);
            HashEntry *elseLabel = makeTemp();
            elseLabel->type = SYMBOL_LABEL;
            HashEntry *endLabel = makeTemp();
            endLabel->type = SYMBOL_LABEL;

            TacList *then = childLists[1];
            TacList *_else = childLists[2];

            code = joinTacList(code, expr);
            appendTacList(code, createTac(
                                        TAC_IFZ,
                                        NULL,
                                        res,
                                        elseLabel));
            code = joinTacList(code, then);
            appendTacList(code, createTac(TAC_JUMP, NULL, endLabel, NULL));
            appendTacList(code, createTac(TAC_LABEL, NULL, elseLabel, NULL));
            code = joinTacList(code, _else);
            appendTacList(code, createTac(TAC_LABEL, NULL, endLabel, NULL));

            break;
        }
        case AST_COMMAND_WHILE: {
            // (AST_WHILE sym: NULL [0]: expr [1]: repeating cmd)
            // Generates:
            // repeat:
            // r = [expr]
            // ifz r end
            // [REPEATING CMD]
            // jmp repeat
            // end:

            TacList *expr = childLists[0];
            HashEntry *res = getListDst(expr);
            HashEntry *repeatLabel = makeTemp();
            repeatLabel->type = SYMBOL_LABEL;
            HashEntry *endLabel = makeTemp();
            endLabel->type = SYMBOL_LABEL;
            TacList *then = childLists[1];

            appendTacList(code, createTac(TAC_LABEL, NULL, repeatLabel, NULL));
            code = joinTacList(code, expr);
            appendTacList(code, createTac(
                                        TAC_IFZ,
                                        NULL,
                                        res,
                                        endLabel));
            code = joinTacList(code, then);
            appendTacList(code, createTac(TAC_JUMP, NULL, repeatLabel, NULL));
            appendTacList(code, createTac(TAC_LABEL, NULL, endLabel, NULL));
            break;
        }
        case AST_COMMAND_PRINT_STRING:
            appendTacList(code, createTac(
                                        TAC_PRINT,
                                        NULL,
                                        ast->symbol,
                                        NULL));
            break;
        case AST_COMMAND_PRINT_EXPR:
            appendTacList(code, createTac(
                                        TAC_PRINT,
                                        NULL,
                                        getListDst(childLists[0]),
                                        NULL));
            break;
        default:
            WARN("Unknown command type %d", type);
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
            list = generateCodeExpr(list, ast, childLists);
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
            list = generateCodeCmds(list, ast, childLists);
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
            // Nothing
            break;
        case AST_IMPL_FUNC: {
            // Prefixes and sufixes with function ideas
            TacList *prefix = createTacList();
            TacList *suffix = createTacList();
            // PREFIX
            appendTacList(prefix, createTac(
                                          TAC_LABEL,
                                          NULL,
                                          ast->symbol, NULL));
            appendTacList(prefix, createTac(
                                          TAC_BEGFUN,
                                          NULL,
                                          ast->symbol, NULL));
            // SUFIX
            appendTacList(suffix, createTac(
                                          TAC_ENDFUN,
                                          NULL,
                                          ast->symbol, NULL));

            for (int i = 0; i < MAX_CHILDREN; i++) {
                list = joinTacList(childLists[i], list);
            }
            list = joinTacList(prefix, list);
            list = joinTacList(list, suffix);

            return list;
        } break;

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
    if (ast->type == AST_COMMAND_IF) {
        // Ifs are handled in generateCodeCmds and
        // have their own code generation logic
        return list;
    }
    if (ast->type == AST_COMMAND_WHILE) {
        // While are handled in generateCodeCmds and
        // have their own code generation logic
        return list;
    }


    for (int i = MAX_CHILDREN - 1; i >= 0; i--) {
        list = joinTacList(childLists[i], list);
    }

    return list;
}
