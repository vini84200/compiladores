//
// Created by vini84200 on 12/9/23.
//

#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "ast.h"
#include "semanticError.h"

typedef struct SemanticAnalyzer_t {
    AST *ast;
    SemanticErrorList *error_list;
} SemanticAnalyzer;

SemanticAnalyzer *newSemanticAnalyzer(struct ast_node *ast);
void destroyAnalyzer(SemanticAnalyzer * analyzer);
void analyzeDeclarations(struct ast_node * declList);
void analyzeProgram(const struct ast_node *ast);

typedef struct SemanticAnalyzerResult_t {
    SemanticErrorList *error_list;
} SemanticAnalyzerResult;

SemanticAnalyzerResult *newSemanticAnalyzerResultFromAnalyzer(const SemanticAnalyzer *analyzer);
void destroySemanticAnalyzerResult(SemanticAnalyzerResult *result);

/**
 * \brief Analyze the semantics of a generated AST, checking for semantic errors
 * \param ast The AST to be analyzed, must be of type AST_PROGRAM
 */
SemanticAnalyzerResult *analyzeSemantics(AST *ast);


void destroySemanticErrorList(SemanticErrorList *pList);
void analyzeImplementations(struct ast_node *implList);
void analyzeCommand(struct ast_node *cmd, struct ParamTypeList_t *pList, Type funcType);
bool checkExpressionType(struct ast_node *expr, TypeBase type, ParamTypeList *pList);
TypeBase getExpressionType(struct ast_node *expr, ParamTypeList *pList);
#endif //SEMANTICS_H
