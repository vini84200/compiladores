//
// Created by vini84200 on 11/30/23.
//

#ifndef AST_H
#define AST_H
#include "hash.h"


#include <stddef.h>

#define MAX_CHILDREN 4


typedef struct ast_node {
    int type;
    HashEntry *symbol;
    struct ast_node *children[MAX_CHILDREN];
} AST;

void open_output_file(char *filename);

AST *createAST(int type, HashEntry *symbol, AST *child0, AST *child1, AST *child2, AST *child3);

void printAST(AST *node, int level);

#define TYPE_INT                    0
#define TYPE_FLOAT                  1
#define TYPE_CHAR                   2

#define AST_PROGRAM                 0
#define AST_DECLARATION_LIST        1
#define AST_VAR_DECLARATION         2
#define AST_FUNC_DECLARATION        3
#define AST_ARRAY_DECLARATION       4
#define AST_ARRAY_VALUES            5
#define AST_PARAM_LIST              6
#define AST_PARAM                   7

#define AST_TYPE_BASE               8
#define AST_TYPE_INT                AST_TYPE_BASE + TYPE_INT
#define AST_TYPE_FLOAT              AST_TYPE_BASE + TYPE_FLOAT
#define AST_TYPE_CHAR               AST_TYPE_BASE + TYPE_CHAR
#define AST_VALUE                   11

#define AST_NO_MORE_DECLARATION     NULL
#define AST_NO_MORE_PARAM           NULL


#define AST_CODE_LIST               12
#define AST_IMPL_FUNC               13
#define AST_EMPTY_COMMAND           14
#define AST_COMMAND_BLOCK           15
#define AST_COMMAND_LIST            16
#define AST_COMMAND_ASSIGN          17
#define AST_COMMAND_ASSIGN_ARRAY    18
#define AST_COMMAND_PRINT_EXPR      19
#define AST_COMMAND_PRINT_STRING    20
#define AST_COMMAND_IF              21
#define AST_COMMAND_WHILE           22

#define AST_EXPR_IDENTIFIER         23
#define AST_EXPR_ARRAY_GET          24
#define AST_EXPR_FUNC_CALL          25
#define AST_EXPR_LIST               26
#define AST_EXPR_LIT_INT            27
#define AST_EXPR_LIT_FLOAT          28
#define AST_EXPR_LIT_CHAR           29
#define AST_EXPR_ADD                30
#define AST_EXPR_SUB                31
#define AST_EXPR_MUL                32
#define AST_EXPR_DIV                33
#define AST_EXPR_LESS               34
#define AST_EXPR_GREATER            35
#define AST_EXPR_LE                 36
#define AST_EXPR_GE                 37
#define AST_EXPR_EQ                 38
#define AST_EXPR_NE                 39
#define AST_EXPR_AND                40
#define AST_EXPR_OR                 41
#define AST_EXPR_NOT                42
#define AST_EXPR_MINUS              43
#define AST_EXPR_READ               44

#define AST_COMMAND_RETURN          45


AST *createASTProgram(AST *declaration_list, AST *code_list);
AST *createASTDeclList(AST *declaration, AST *declaration_list);
AST *createASTVarDecl(AST *type, HashEntry *identifier, AST *value);
AST *createASTType(int type);

AST *createASTValue(HashEntry *value);
AST *createASTArrayDecl(AST *type, HashEntry *identifier, HashEntry *size, AST *value);
AST *createASTArrayValues(AST *value, AST *values);
AST *createASTFuncDecl(AST* type, HashEntry* identifier, AST* param_list);
AST *createASTParamList(AST* param, AST* param_list);
AST *createASTParam(AST* type, HashEntry* identifier);

AST *createASTCodeList(AST *code, AST *code_list);
AST *createASTImplFunc(HashEntry *identifier, AST* command);
AST *createASTEmptyCommand();
AST *createASTCommandBlock(AST* command_list);
AST *createASTCommandList(AST* command, AST* command_list);

AST *createASTCommandAssign(HashEntry *identifier, AST *expr);
AST *createASTCommandAssignArray(HashEntry *identifier, AST *expr_index, AST *expr_value);
AST *createASTCommandPrintExpr(AST *expr);
AST *createASTCommandPrintString(HashEntry *string);
AST *createASTCommandIf(AST *expr, AST *command_if, AST *command_else);
AST *createASTCommandWhile(AST *expr, AST *command_while);

AST *createASTExprVar(HashEntry *identifier);
AST *createASTExprArrayGet(HashEntry *identifier, AST *expr_index);
AST *createASTExprFuncCall(HashEntry *identifier, AST *expr_list);
AST *createASTExprList(AST *expr, AST *expr_list);
AST *createASTExprLitInt(HashEntry *value);
AST *createASTExprLitFloat(HashEntry *value);
AST *createASTExprLitChar(HashEntry *value);
AST *createASTExprAdd(AST *expr0, AST *expr1);
AST *createASTExprSub(AST *expr0, AST *expr1);
AST *createASTExprMul(AST *expr0, AST *expr1);
AST *createASTExprDiv(AST *expr0, AST *expr1);
AST *createASTExprLess(AST *expr0, AST *expr1);
AST *createASTExprGreater(AST *expr0, AST *expr1);
AST *createASTExprLE(AST *expr0, AST *expr1);
AST *createASTExprGE(AST *expr0, AST *expr1);
AST *createASTExprEQ(AST *expr0, AST *expr1);
AST *createASTExprNE(AST *expr0, AST *expr1);
AST *createASTExprAnd(AST *expr0, AST *expr1);
AST *createASTExprOr(AST *expr0, AST *expr1);
AST *createASTExprNot(AST *expr);
AST *createASTExprMinus(AST *expr);
AST *createASTExprRead(AST *type);

AST *createASTCommandReturn(AST *expr);


#endif //AST_H
