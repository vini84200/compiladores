//
// Created by vini84200 on 11/30/23.
//

#ifndef AST_H
#define AST_H
#include "hash.h"


#include <stddef.h>

struct YYLTYPE;
#define MAX_CHILDREN 4

typedef enum AST_Type_t {
    // Structure
    AST_PROGRAM,
    AST_DECLARATION_LIST,
    AST_CODE_LIST,
    AST_IMPL_FUNC,

    // Definitions
    AST_VAR_DECLARATION,
    AST_FUNC_DECLARATION,
    AST_ARRAY_DECLARATION,
    AST_ARRAY_VALUES,
    AST_PARAM_LIST,
    AST_PARAM,

    // Types
    AST_TYPE_INT,
    AST_TYPE_FLOAT,
    AST_TYPE_CHAR,
    AST_VALUE,

    // Commnads
    AST_EMPTY_COMMAND,
    AST_COMMAND_BLOCK,
    AST_COMMAND_LIST,
    AST_COMMAND_ASSIGN,
    AST_COMMAND_ASSIGN_ARRAY,
    AST_COMMAND_PRINT_EXPR,
    AST_COMMAND_PRINT_STRING,
    AST_COMMAND_IF,
    AST_COMMAND_WHILE,
    AST_COMMAND_RETURN,

    // Exprs
    AST_EXPR_IDENTIFIER,
    AST_EXPR_ARRAY_GET,
    AST_EXPR_FUNC_CALL,
    AST_EXPR_LIST,
    AST_EXPR_LIT_INT,
    AST_EXPR_LIT_FLOAT,
    AST_EXPR_LIT_CHAR,
    AST_EXPR_ADD,
    AST_EXPR_SUB,
    AST_EXPR_MUL,
    AST_EXPR_DIV,
    AST_EXPR_LESS,
    AST_EXPR_GREATER,
    AST_EXPR_LE,
    AST_EXPR_GE,
    AST_EXPR_EQ,
    AST_EXPR_NE,
    AST_EXPR_AND,
    AST_EXPR_OR,
    AST_EXPR_NOT,
    AST_EXPR_MINUS,
    AST_EXPR_READ,
} AST_Type;

#define AST_TYPE_BASE AST_TYPE_INT


typedef struct ast_node {
    AST_Type type;
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


#define AST_NO_MORE_DECLARATION NULL
#define AST_NO_MORE_PARAM NULL


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
