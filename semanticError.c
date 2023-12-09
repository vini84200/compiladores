//
// Created by vini84200 on 12/9/23.
//

#include "semanticError.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
SemanticError *newSemanticError(int error_code, char *message, Span *span, Span *secondary_span) {
    SemanticError *semantic_error = (SemanticError *) malloc(sizeof(SemanticError));
    semantic_error->error_code = error_code;
    semantic_error->message = message;
    semantic_error->span = span;
    semantic_error->secondary_span = secondary_span;
    semantic_error->next = NULL;
    return semantic_error;
}

#define FORMAT_(ptr, ...) saprintf(ptr, __VA_ARGS__)

SemanticError *newUndefIdentifierSemanticError(char *identifier, Span *span) {
    char *message;
    asprintf(&message, "Undefined identifier %s", identifier);
    return newSemanticError(SEMANTIC_ERROR_UNDEF_IDENTIFIER, message, span, NULL);
}
SemanticError *newRedefIdentifierSemanticError(char *identifier, Span *span, Span *original_declaration_span) {
    char *message;
    asprintf(&message, "Redefinition of identifier %s", identifier);
    return newSemanticError(SEMANTIC_ERROR_REDEF_IDENTIFIER, message, span, original_declaration_span);
}
SemanticError *newGlobalParamConflictSemanticError(char *identifier, Span *span, Span *original_declaration_span) {
    char *message;
    asprintf(&message, "Global variable %s conflicts with parameter name", identifier);
    return newSemanticError(SEMANTIC_ERROR_GLOBAL_PARAM_CONFLICT, message, span, original_declaration_span);
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