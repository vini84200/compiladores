//
// Created by vini84200 on 12/9/23.
//

#include "semanticError.h"
#include "types.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
SemanticError *newSemanticError(SemanticErrorType error_code, char *message, Span *span, Span *secondary_span) {
    SemanticError *semantic_error = (SemanticError *) malloc(sizeof(SemanticError));
    semantic_error->error_code = error_code;
    semantic_error->message = message;
    semantic_error->span = span;
    semantic_error->span_text = "";
    semantic_error->secondary_span = secondary_span;
    semantic_error->secondary_span_text = "";
    semantic_error->next = NULL;
    return semantic_error;
}

char *getTypeBaseName(TypeBase type);
SemanticError *newUndefIdentifierSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Undefined identifier %s", identifier);
    return newSemanticErrorWithMessages(SEMANTIC_ERROR_UNDEF_IDENTIFIER, message, span, NULL, "Undefined identifier", "");
}
SemanticError *newRedefIdentifierSemanticError(char *identifier, Span *span, Span *original_declaration_span) {
    char *message;
    asprintf(&message, "Redefinition of identifier %s", identifier);
    return newSemanticErrorWithMessages(SEMANTIC_ERROR_REDEF_IDENTIFIER, message, span, original_declaration_span, "Redefinition of identifier", "Original declaration");
}
SemanticError *newGlobalParamConflictSemanticError(char *identifier, Span *span, Span *original_declaration_span) {
    char *message;
    asprintf(&message, "Global variable %s conflicts with parameter name", identifier);
    return newSemanticErrorWithMessages(SEMANTIC_ERROR_GLOBAL_PARAM_CONFLICT, message, span, original_declaration_span, "Parameter to global conflict", "Original declaration");
}
SemanticErrorList *newSemanticErrorList() {
    SemanticErrorList *list = (SemanticErrorList *) malloc(sizeof(SemanticErrorList));
    list->first = NULL;
    list->last = NULL;
    return list;
}
void semanticErrorListAppend(SemanticErrorList *list, SemanticError *error) {
    if (list->first == NULL) {
        list->first = error;
    } else {
        list->last->next = error;
    }
    list->last = error;
}
SemanticErrorIterator *newSemanticErrorIterator(const SemanticErrorList *list) {
    SemanticErrorIterator *iterator = (SemanticErrorIterator *) malloc(sizeof(SemanticErrorIterator));
    iterator->next = list->first;
    return iterator;
}
SemanticError *semanticErrorIteratorNext(SemanticErrorIterator *iterator) {
    SemanticError *current = iterator->next;
    if (current != NULL) {
        iterator->next = current->next;
    }
    return current;
}
bool semanticErrorIteratorDone(SemanticErrorIterator *iterator) {
    return iterator->next == NULL;
}
SemanticError *newIsNotAFunctionSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Identifier %s is not a function", identifier);
    return newSemanticError(SEMANTIC_ERROR_IS_NOT_A_FUNCTION, message, span, NULL);
}
SemanticError *newFunctionReimplementationSemanticError(char *identifier, Span *span, Span *original_impl_span) {
    char *message;
    asprintf(&message, "Function %s is already implemented", identifier);
    return newSemanticError(SEMANTIC_ERROR_FUNCTION_REIMPLEMENTATION, message, span, original_impl_span);
}
SemanticError *newFunctionNotImplementedSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Function %s is not implemented", identifier);
    return newSemanticError(SEMANTIC_ERROR_FUNCTION_NOT_IMPLEMENTED, message, span, NULL);
}
SemanticError *newFunctionNotDeclaredSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Function %s is not declared", identifier);
    return newSemanticError(SEMANTIC_ERROR_FUNCTION_NOT_DECLARED, message, span, NULL);
}
SemanticError *newAssignToParamSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot assign to parameter %s, parameters are read-only", identifier);
    return newSemanticError(SEMANTIC_ERROR_ASSIGN_TO_PARAM, message, span, NULL);
}
SemanticError *newAssignToArraySemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot assign to array %s, arrays cannot be directly assigned, use an index", identifier);
    return newSemanticError(SEMANTIC_ERROR_ASSIGN_TO_ARRAY, message, span, NULL);
}
SemanticError *newAssignToFunctionSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot assign to function %s, functions cannot be assigned", identifier);
    return newSemanticError(SEMANTIC_ERROR_ASSIGN_TO_FUNCTION, message, span, NULL);
}
SemanticError *newReadFromArraySemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot read from array %s, arrays cannot be directly read, use an index", identifier);
    return newSemanticError(SEMANTIC_ERROR_READ_FROM_ARRAY, message, span, NULL);
}
SemanticError *newReadFromFunctionSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot read from function %s, functions cannot be read. Call the function instead", identifier);
    return newSemanticError(SEMANTIC_ERROR_READ_FROM_FUNCTION, message, span, NULL);
}
SemanticError *newIndexScalarSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot index scalar %s, only arrays can be indexed", identifier);
    return newSemanticError(SEMANTIC_ERROR_INDEX_SCALAR, message, span, NULL);
}
SemanticError *newIndexFunctionSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot index function %s, only arrays can be indexed", identifier);
    return newSemanticError(SEMANTIC_ERROR_INDEX_FUNCTION, message, span, NULL);
}
SemanticError *newCallScalarSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot call scalar %s, only functions can be called", identifier);
    return newSemanticError(SEMANTIC_ERROR_CALL_SCALAR, message, span, NULL);
}
SemanticError *newCallArraySemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Cannot call array %s, only functions can be called", identifier);
    return newSemanticError(SEMANTIC_ERROR_CALL_ARRAY, message, span, NULL);
}

SemanticError *newWrongOperandTypeSemanticErrorBinaryLeft(char *identifier, char *expected, TypeBase got, Span *span) {
    char *message;
    asprintf(&message, "Wrong operand type for operator %s on left side, expected %s, got %s", identifier, expected,
             getTypeBaseName(got));
    return newSemanticError(SEMANTIC_ERROR_WRONG_OPERAND_TYPE_BINARY, message, span, NULL);
}
SemanticError *newWrongOperandTypeSemanticErrorBinaryRight(char *identifier, char *expected, TypeBase got, Span *span) {
    char *message;
    asprintf(&message, "Wrong operand type for operator %s on right side, expected %s, got %s", identifier, expected,
             getTypeBaseName(got));
    return newSemanticError(SEMANTIC_ERROR_WRONG_OPERAND_TYPE_BINARY, message, span, NULL);
}
SemanticError *newWrongOperandTypeSemanticErrorUnary(char *identifier, char *expected, TypeBase got, Span *span) {
    char *message;
    asprintf(&message, "Wrong operand type for operator %s, expected %s, got %s", identifier, expected,
             getTypeBaseName(got));
    return newSemanticError(SEMANTIC_ERROR_WRONG_OPERAND_TYPE_BINARY, message, span, NULL);
}
SemanticError *newAssignTypeMismatchSemanticError(char *identifier, TypeBase expected, TypeBase got, Span *span) {
    char *message;
    asprintf(&message, "Type Mismatch: Cannot assign %s to %s, should be %s", getTypeBaseName(got), identifier,
             getTypeBaseName(expected));
    return newSemanticError(SEMANTIC_ERROR_ASSIGN_TYPE_MISMATCH, message, span, NULL);
}
SemanticError *newReturnWrongTypeSemanticError(TypeBase expected, TypeBase got, Span *span) {
    char *message;
    asprintf(&message, "Type Mismatch: Cannot return %s, should be %s", getTypeBaseName(got),
             getTypeBaseName(expected));
    return newSemanticError(SEMANTIC_ERROR_WRONG_RETURN_TYPE, message, span, NULL);
}
SemanticError *newIndexNotIntSemanticError(char *identifier, TypeBase got, Span *span) {
    char *message;
    asprintf(&message, "Type Mismatch: Cannot index %s with %s, should be int", identifier, getTypeBaseName(got));
    return newSemanticError(SEMANTIC_INDEX_NOT_INT, message, span, NULL);
}
SemanticError *newWrongArgCountSemanticError(char *func_identifier, int expected, int got, Span *span, Span *secondary_span) {
    char *message;
    asprintf(&message, "Wrong number of arguments for function %s, expected %d, got %d", func_identifier, expected,
             got);
    return newSemanticErrorWithMessages(SEMANTIC_ERROR_WRONG_ARG_COUNT, message, span, secondary_span, "Wrong number of arguments", "Function declaration");
}
SemanticError *newWrongArgTypeSemanticError(char *func_identifier, char *arg_identifier, TypeBase expected, TypeBase got, Span *span) {
    char *message;
    asprintf(&message, "Wrong type for argument %s of function %s, expected %s, got %s", arg_identifier, func_identifier, getTypeBaseName(expected), getTypeBaseName(got));
    return newSemanticError(SEMANTIC_ERROR_WRONG_ARG_TYPE, message, span, NULL);
}
SemanticError *newSemanticErrorWithMessages(SemanticErrorType error_code, char *message, Span *span, Span *secondary_span, char *span_text, char *secondary_span_text) {
    SemanticError *semantic_error = (SemanticError *) malloc(sizeof(SemanticError));
    semantic_error->error_code = error_code;
    semantic_error->message = message;
    semantic_error->span = span;
    semantic_error->span_text = span_text;
    semantic_error->secondary_span = secondary_span;
    semantic_error->secondary_span_text = secondary_span_text;
    semantic_error->next = NULL;
    return semantic_error;
}
SemanticError *newInitializedWithWrongTypeSemanticError(char *identifier, TypeBase expected, TypeBase got, Span *span) {
    char *message;
    asprintf(&message, "Type Mismatch: Cannot initialize %s with %s, should be %s", identifier, getTypeBaseName(got),
             getTypeBaseName(expected));
    return newSemanticErrorWithMessages(
            SEMANTIC_ERROR_INITIAIZED_WITH_WRONG_TYPE,
            message,
            span,
            NULL,
            "Type Mismatch",
            "");
}
SemanticError *newArrayInitializedWithTooManyValuesSemanticError(char *identifier, int expected, int got, Span *span) {
    char *message;
    asprintf(&message, "Array size Mismatch: Cannot initialize %s with %d values, should be at most %d", identifier, got,
             expected);
    return newSemanticErrorWithMessages(
            SEMANTIC_ERROR_ARRAY_INITIAIZED_WITH_TOO_MANY_VALUES,
            message,
            span,
            NULL,
            "Array size Mismatch",
            "");
}

char *getTypeBaseName(TypeBase type) {
    switch (type) {
        case TYPE_BASE_INT:
            return "int";
        case TYPE_BASE_FLOAT:
            return "float";
        case TYPE_BASE_BOOL:
            return "boolean";
        case TYPE_BASE_ERROR:
            return "error";
        default:
            return "unknown";
        case TYPE_BASE_CHAR:
            return "char";
        case TYPE_BASE_STRING:
            return "string";
    }
}
