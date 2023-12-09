//
// Created by vini84200 on 12/9/23.
//

#include "semantics.h"


#include "errorHandler.h"
#include "lib.h"


#include <stdlib.h>

SemanticAnalyzer *analyzer;

void destroySemanticErrorList(SemanticErrorList *pList);
void semanticTry(SemanticError *error) {
    SemanticError *current = error;
    while (current != NULL) {
        SemanticError *next = current->next;
        semanticErrorListAppend(analyzer->error_list, current);
        current = next;
    }
}

void analyzeDeclarations(AST *declList) {
    ASTListIterator *iterator = createASTListIterator(declList);
    while (!ASTIteratorDone(iterator)) {
        const AST *decl = getNextAST(iterator);
        if (decl->type == AST_VAR_DECLARATION) {
            // VAR_DECLARATION( sym=id, $0=type, $1=value )
            Type *type = newScalarType(getTypeBaseFromASTType(decl->children[0]));
            semanticTry((setGlobalBound(decl->symbol, type, emptySpan())));
            continue;
        }
        if (decl->type == AST_ARRAY_DECLARATION) {
            // ARRAY_DECLARATION( sym=id, $0=type, $1=size, $2=list_of_values )
            const int size = atoi(decl->children[1]->symbol->value);
            Type *type = newArrayType(getTypeBaseFromASTType(decl->children[0]), size);
            semanticTry(setGlobalBound(decl->symbol, type, emptySpan()));
            continue;
        }
        if (decl->type == AST_FUNC_DECLARATION) {
            // FUNC_DECLARATION( sym=id, $0=type, $1=param_list)
            TypeBase returnType = getTypeBaseFromASTType(decl->children[0]);
            ParamTypeList *param_list = newParamTypeList();
            ASTListIterator *param_iterator = createASTListIterator(decl->children[1]);
            while (!ASTIteratorDone(param_iterator)) {
                const AST *param = getNextAST(param_iterator);
                // PARAM( sym=id, $0=type )
                TypeBase param_type = getTypeBaseFromASTType(param->children[0]);
                Type *type = newScalarType(param_type);
                paramTypeListAppend(param_list, newParamType(type, param->symbol));
                // Bind the parameter
                semanticTry(setParamBound(param->symbol, type, decl->symbol));

            }
            destroyASTListIterator(param_iterator);
            Type *type = newFunctionType(returnType, param_list);
            semanticTry(setGlobalBound(decl->symbol, type, emptySpan()));
            continue;
        }
    }
    destroyASTListIterator(iterator);

    // Check if all the identifiers are defined
    SymbolIterator *symbol_iterator = createSymbolIterator(getSymbolTable(), SYMBOL_IDENTIFIER);
    while (!SymbolIteratorDone(symbol_iterator)) {
        const HashEntry *entry = getNextSymbol(symbol_iterator);
        if (entry == NULL) {
            printf("NULL entry\n");
            continue;
        }
        if (entry->identifier == NULL) {
            semanticTry(newUndefIdentifierSemanticError(entry->value, emptySpan()));
        } else {
            printf("Identifier %s is defined\n", entry->value);
            printf("Type: %s\n", getSymbolName(entry->type));
        }
    }
    destroySymbolIterator(symbol_iterator);
}

SemanticAnalyzer *newSemanticAnalyzer(AST *ast) {
    SemanticAnalyzer *analyzer = (SemanticAnalyzer *) malloc(sizeof(SemanticAnalyzer));
    analyzer->ast = ast;
    analyzer->error_list = newSemanticErrorList();
    return analyzer;
}


void analyzeProgram(const AST *ast) {
    // First collect all declarations
    analyzeDeclarations(ast->children[0]);

    // Then check the implementations
}

SemanticAnalyzerResult *newSemanticAnalyzerResultFromAnalyzer(const SemanticAnalyzer *analyzer) {
    SemanticAnalyzerResult *result = (SemanticAnalyzerResult *) malloc(sizeof(SemanticAnalyzerResult));
    result->error_list = analyzer->error_list;
    return result;
}

void destroyAnalyzer(SemanticAnalyzer *analyzer) {
    free(analyzer);
    // We don't destroy the AST because it's not ours
    // We don't destroy the error list because it will be used by the result
}

SemanticAnalyzerResult *anylzeSemantic(AST *ast) {
    if (ast == NULL) {
        criticalError("Cannot analyze a NULL AST");
    }
    if (ast->type == AST_PROGRAM) {
        // Create a new analyzer
        analyzer = newSemanticAnalyzer(ast);
        analyzeProgram(ast);
        SemanticAnalyzerResult *result = newSemanticAnalyzerResultFromAnalyzer(analyzer);
        destroyAnalyzer(analyzer);
        return result;
    }
    // The AST is not a program, so we can't analyze it
    criticalError("Semantic analysis can only be performed on a program AST");
}
void destroySemanticAnalyzerResult(SemanticAnalyzerResult *result) {
    destroySemanticErrorList(result->error_list);
    free(result);
}
void destroySemanticErrorList(SemanticErrorList *pList) {
    SemanticErrorIterator *iterator = newSemanticErrorIterator(pList);
    SemanticError *error;
    while ((error = semanticErrorIteratorNext(iterator)) != NULL) {
        free(error);
    }
    free(iterator);
    free(pList);
}
