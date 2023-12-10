//
// Created by vini84200 on 12/9/23.
//

#ifndef SEMANTICERROR_H
#define SEMANTICERROR_H


#include "span.h"
#include <stdbool.h>

typedef enum TypeBase_t TypeBase;

typedef enum SemanticErrorType_t {
    SEMANTIC_ERROR_UNDEF_IDENTIFIER,
    SEMANTIC_ERROR_REDEF_IDENTIFIER,
    SEMANTIC_ERROR_GLOBAL_PARAM_CONFLICT,
    SEMANTIC_ERROR_IS_NOT_A_FUNCTION,
    SEMANTIC_ERROR_FUNCTION_REIMPLEMENTATION,
    SEMANTIC_ERROR_FUNCTION_NOT_IMPLEMENTED,
    SEMANTIC_ERROR_FUNCTION_NOT_DECLARED,
    SEMANTIC_ERROR_ASSIGN_TO_PARAM,
    SEMANTIC_ERROR_ASSIGN_TO_ARRAY,
    SEMANTIC_ERROR_ASSIGN_TO_FUNCTION,
    SEMANTIC_ERROR_READ_FROM_ARRAY,
    SEMANTIC_ERROR_READ_FROM_FUNCTION,
    SEMANTIC_ERROR_INDEX_SCALAR,
    SEMANTIC_ERROR_INDEX_FUNCTION,
    SEMANTIC_ERROR_CALL_SCALAR,
    SEMANTIC_ERROR_CALL_ARRAY,
    SEMANTIC_ERROR_WRONG_OPERAND_TYPE_BINARY,
    SEMANTIC_ERROR_ASSIGN_TYPE_MISMATCH,
    SEMANTIC_INDEX_NOT_INT,
    SEMANTIC_ERROR_WRONG_RETURN_TYPE,
    SEMANTIC_ERROR_WRONG_ARG_COUNT,
    SEMANTIC_ERROR_WRONG_ARG_TYPE
} SemanticErrorType;


typedef struct SemanticError_t {
    SemanticErrorType error_code;
    char *message;
    Span *span;
    Span *secondary_span;


    struct SemanticError_t *next;
} SemanticError;

SemanticError *newSemanticError(SemanticErrorType error_code, char *message, Span *span, Span *secondary_span);
SemanticError *newUndefIdentifierSemanticError(char *identifier, Span *span);
SemanticError *newRedefIdentifierSemanticError(char *identifier, Span *span, Span *original_declaration_span);
SemanticError *newGlobalParamConflictSemanticError(char *identifier, Span *span, Span *original_declaration_span);
SemanticError *newIsNotAFunctionSemanticError(char *identifier, Span *span);
SemanticError *newFunctionReimplementationSemanticError(char *identifier, Span *span, Span *original_impl_span);
SemanticError *newFunctionNotImplementedSemanticError(char *identifier, Span *span);
SemanticError *newFunctionNotDeclaredSemanticError(char *identifier, Span *span);
SemanticError *newAssignToParamSemanticError(char *identifier, Span *span);
SemanticError *newAssignToArraySemanticError(char *identifier, Span *span);
SemanticError *newAssignToFunctionSemanticError(char *identifier, Span *span);
SemanticError *newReadFromArraySemanticError(char *identifier, Span *span);
SemanticError *newReadFromFunctionSemanticError(char *identifier, Span *span);
SemanticError *newIndexScalarSemanticError(char *identifier, Span *span);
SemanticError *newIndexFunctionSemanticError(char *identifier, Span *span);
SemanticError *newCallScalarSemanticError(char *identifier, Span *span);
SemanticError *newCallArraySemanticError(char *identifier, Span *span);
SemanticError *newWrongOperandTypeSemanticErrorBinaryLeft(char *identifier, char *expected, TypeBase got, Span *span);
SemanticError *newWrongOperandTypeSemanticErrorBinaryRight(char *identifier, char *expected, TypeBase got, Span *span);
SemanticError *newWrongOperandTypeSemanticErrorUnary(char *identifier, char *expected, TypeBase got, Span *span);
SemanticError *newAssignTypeMismatchSemanticError(char *identifier, TypeBase expected, TypeBase got, Span *span);
SemanticError *newIndexNotIntSemanticError(char *identifier, TypeBase got, Span *span);
SemanticError *newReturnWrongTypeSemanticError(TypeBase expected, TypeBase got, Span *span);
SemanticError *newWrongArgCountSemanticError(char *func_identifier,int expected, int got, Span *span);
SemanticError *newWrongArgTypeSemanticError(char *func_identifier, char *arg_identifier, TypeBase expected, TypeBase got, Span *span);

typedef struct SemanticErrorList_t {
    SemanticError *first;
    SemanticError *last;
} SemanticErrorList;

SemanticErrorList *newSemanticErrorList();
void semanticErrorListAppend(SemanticErrorList *list, SemanticError *error);

typedef struct SemanticErrorIterator_t {
    SemanticError *next;
} SemanticErrorIterator;

SemanticErrorIterator *newSemanticErrorIterator(const SemanticErrorList *list);
SemanticError *semanticErrorIteratorNext(SemanticErrorIterator *iterator);
bool semanticErrorIteratorDone(SemanticErrorIterator *iterator);



#endif //SEMANTICERROR_H
