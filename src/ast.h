//
// Created by vini84200 on 11/30/23.
//

#ifndef AST_H
#define AST_H
#include "hash.h"


#include <stddef.h>

struct YYLTYPE;
#define MAX_CHILDREN 4


typedef struct ast_node {
    int type;
    HashEntry *symbol;
    struct ast_node *children[MAX_CHILDREN];
    Span *span;
} AST;

void open_output_file(char *filename);

AST *createASTnoSpan(int type, HashEntry *symbol, AST *child0, AST *child1, AST *child2, AST *child3);
AST *createAST(int type, HashEntry *symbol, AST *child0, AST *child1, AST *child2, AST *child3, struct YYLTYPE location);
void destroyAST(AST *node);

void printAST(AST *node, int level);

#define TYPE_INT 0
#define TYPE_FLOAT 1
#define TYPE_CHAR 2

#define AST_PROGRAM 0
#define AST_DECLARATION_LIST 1
#define AST_VAR_DECLARATION 2
#define AST_FUNC_DECLARATION 3
#define AST_ARRAY_DECLARATION 4
#define AST_ARRAY_VALUES 5
#define AST_PARAM_LIST 6
#define AST_PARAM 7

#define AST_TYPE_BASE 8
#define AST_TYPE_INT AST_TYPE_BASE + TYPE_INT
#define AST_TYPE_FLOAT AST_TYPE_BASE + TYPE_FLOAT
#define AST_TYPE_CHAR AST_TYPE_BASE + TYPE_CHAR
#define AST_VALUE 11

#define AST_NO_MORE_DECLARATION NULL
#define AST_NO_MORE_PARAM NULL


#define AST_CODE_LIST 12
#define AST_IMPL_FUNC 13
#define AST_EMPTY_COMMAND 14
#define AST_COMMAND_BLOCK 15
#define AST_COMMAND_LIST 16
#define AST_COMMAND_ASSIGN 17
#define AST_COMMAND_ASSIGN_ARRAY 18
#define AST_COMMAND_PRINT_EXPR 19
#define AST_COMMAND_PRINT_STRING 20
#define AST_COMMAND_IF 21
#define AST_COMMAND_WHILE 22

#define AST_EXPR_IDENTIFIER 23
#define AST_EXPR_ARRAY_GET 24
#define AST_EXPR_FUNC_CALL 25
#define AST_EXPR_LIST 26
#define AST_EXPR_LIT_INT 27
#define AST_EXPR_LIT_FLOAT 28
#define AST_EXPR_LIT_CHAR 29
#define AST_EXPR_ADD 30
#define AST_EXPR_SUB 31
#define AST_EXPR_MUL 32
#define AST_EXPR_DIV 33
#define AST_EXPR_LESS 34
#define AST_EXPR_GREATER 35
#define AST_EXPR_LE 36
#define AST_EXPR_GE 37
#define AST_EXPR_EQ 38
#define AST_EXPR_NE 39
#define AST_EXPR_AND 40
#define AST_EXPR_OR 41
#define AST_EXPR_NOT 42
#define AST_EXPR_MINUS 43
#define AST_EXPR_READ 44

#define AST_COMMAND_RETURN 45


AST *createASTProgram(AST *declaration_list, AST *code_list, struct YYLTYPE loc);
AST *createASTDeclList(AST *declaration, AST *declaration_list, struct YYLTYPE loc);
AST *createASTVarDecl(AST *type, HashEntry *identifier, AST *value, struct YYLTYPE loc);
AST *createASTType(int type, struct YYLTYPE loc);

AST *createASTValue(HashEntry *value, struct YYLTYPE loc);
AST *createASTArrayDecl(AST *type, HashEntry *identifier, HashEntry *size, AST *value, struct YYLTYPE loc);
AST *createASTArrayValues(AST *value, AST *values, struct YYLTYPE loc);
AST *createASTFuncDecl(AST *type, HashEntry *identifier, AST *param_list, struct YYLTYPE loc);
AST *createASTParamList(AST *param, AST *param_list, struct YYLTYPE loc);
AST *createASTParam(AST *type, HashEntry *identifier, struct YYLTYPE loc);

AST *createASTCodeList(AST *code, AST *code_list, struct YYLTYPE loc);
AST *createASTImplFunc(HashEntry *identifier, AST *command, struct YYLTYPE loc);
AST *createASTEmptyCommand(struct YYLTYPE loc);
AST *createASTCommandBlock(AST *command_list, struct YYLTYPE loc);
AST *createASTCommandList(AST *command, AST *command_list, struct YYLTYPE loc);

AST *createASTCommandAssign(HashEntry *identifier, AST *expr, struct YYLTYPE loc);
AST *createASTCommandAssignArray(HashEntry *identifier, AST *expr_index, AST *expr_value, struct YYLTYPE loc);
AST *createASTCommandPrintExpr(AST *expr, struct YYLTYPE loc);
AST *createASTCommandPrintString(HashEntry *string, struct YYLTYPE loc);
AST *createASTCommandIf(AST *expr, AST *command_if, AST *command_else, struct YYLTYPE loc);
AST *createASTCommandWhile(AST *expr, AST *command_while, struct YYLTYPE loc);

AST *createASTExprVar(HashEntry *identifier, struct YYLTYPE loc);
AST *createASTExprArrayGet(HashEntry *identifier, AST *expr_index, struct YYLTYPE loc);
AST *createASTExprFuncCall(HashEntry *identifier, AST *expr_list, struct YYLTYPE loc);
AST *createASTExprList(AST *expr, AST *expr_list, struct YYLTYPE loc);
AST *createASTExprLitInt(HashEntry *value, struct YYLTYPE loc);
AST *createASTExprLitFloat(HashEntry *value, struct YYLTYPE loc);
AST *createASTExprLitChar(HashEntry *value, struct YYLTYPE loc);
AST *createASTExprAdd(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprSub(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprMul(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprDiv(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprLess(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprGreater(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprLE(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprGE(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprEQ(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprNE(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprAnd(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprOr(AST *expr0, AST *expr1, struct YYLTYPE loc);
AST *createASTExprNot(AST *expr, struct YYLTYPE loc);
AST *createASTExprMinus(AST *expr, struct YYLTYPE loc);
AST *createASTExprRead(AST *type, struct YYLTYPE loc);

AST *createASTCommandReturn(AST *expr, struct YYLTYPE loc);

typedef struct {
    AST *next;
    int index;
} ASTListIterator;

ASTListIterator *createASTListIterator(AST *list);
AST *getNextAST(ASTListIterator *iterator);
bool ASTIteratorDone(ASTListIterator *iterator);
void destroyASTListIterator(ASTListIterator *iterator);

void astPrintDebug(AST *node, int level);

#endif//AST_H
