//
// Created by vini84200 on 12/9/23.
//

#ifndef SEMANTICERROR_H
#define SEMANTICERROR_H

#define SEMANTIC_ERROR_UNDEF_IDENTIFIER 1
#define SEMANTIC_ERROR_REDEF_IDENTIFIER 2
#define SEMANTIC_ERROR_GLOBAL_PARAM_CONFLICT 3
#include "span.h"


#include <stdbool.h>

typedef struct SemanticError_t {
    int error_code;
    char *message;
    Span *span;
    Span *secondary_span;


    struct SemanticError_t *next;
} SemanticError;

SemanticError *newSemanticError(int error_code, char *message, Span *span, Span *secondary_span);
SemanticError *newUndefIdentifierSemanticError(char *identifier, Span *span);
SemanticError *newRedefIdentifierSemanticError(char *identifier, Span *span, Span *original_declaration_span);
SemanticError *newGlobalParamConflictSemanticError(char *identifier, Span *span, Span *original_declaration_span);

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
