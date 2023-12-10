//
// Created by vini84200 on 12/9/23.
//

#include "types.h"

#include "ast.h"
#include "errorHandler.h"
#include "hash.h"
#include "semanticError.h"


#include <stddef.h>
#include <stdlib.h>

TypeBase getTypeBaseFromASTType(const struct ast_node *type) {
    switch (type->type) {
        case AST_TYPE_INT:
            return TYPE_BASE_INT;
        case AST_TYPE_FLOAT:
            return TYPE_BASE_FLOAT;
        case AST_TYPE_CHAR:
            return TYPE_BASE_CHAR;
        case AST_VALUE:
        {
            switch (type->symbol->type) {
                case SYMBOL_LIT_INT:
                    return TYPE_BASE_INT;
                case SYMBOL_LIT_FLOAT:
                    return TYPE_BASE_FLOAT;
                case SYMBOL_LIT_CHAR:
                    return TYPE_BASE_CHAR;
                default:
                    criticalError("Cannot get type base from AST");
            }
        }
        case AST_EXPR_LIT_INT:
            return TYPE_BASE_INT;
        case AST_EXPR_LIT_FLOAT:
            return TYPE_BASE_FLOAT;
        case AST_EXPR_LIT_CHAR:
            return TYPE_BASE_CHAR;

        default:
            criticalError("Cannot get type base from AST");
    }
}
Type *newScalarType(TypeBase base) {
    Type *type = (Type *) malloc(sizeof(Type));
    type->plural = TYPE_NATURE_SCALAR;
    type->base = base;
    type->size = 1;
    type->arg_list = NULL;
    return type;
}
Type *newArrayType(TypeBase base, int size) {
    Type *type = (Type *) malloc(sizeof(Type));
    type->plural = TYPE_NATURE_ARRAY;
    type->base = base;
    type->size = size;
    type->arg_list = NULL;
    return type;
}
Bound *newGlobalBound() {
    Bound *bound = (Bound *) malloc(sizeof(Bound));
    bound->bound_type = BOUND_TYPE_GLOBAL;
    bound->symbol = NULL;
    return bound;
}
Bound *newParamBound(struct HashEntry_t *function_identifier) {
    Bound *bound = (Bound *) malloc(sizeof(Bound));
    bound->bound_type = BOUND_TYPE_PARAM;
    bound->symbol = function_identifier;
    return bound;
}
Identifier *newIdentifier(char *name, Type *type, Bound *bound, Span *declaration_span) {
    Identifier *identifier = (Identifier *) malloc(sizeof(Identifier));
    identifier->name = name;
    identifier->type = type;
    identifier->bound = bound;
    identifier->declaration_span = declaration_span;
    identifier->next = NULL;
    return identifier;
}
SemanticError *setGlobalBound(struct HashEntry_t *symbol, Type *type, Span *declaration_span) {
    if (symbol == NULL) {
        criticalError("Cannot set a global bound to a NULL symbol");
    }
    if (type == NULL) {
        criticalError("Cannot set a global bound to a NULL type");
    }
    if (symbol->identifier != NULL) {
        // The symbol already has a bound set
        // As we are setting a global bound, this is an error

        if (symbol->identifier->bound->bound_type == BOUND_TYPE_GLOBAL) {
            // The symbol already has a global bound set
            // This is an error
            return newRedefIdentifierSemanticError(symbol->identifier->name, declaration_span,
                                                   symbol->identifier->declaration_span);
        } else {
            // The symbol already has a param bound set
            // This is an error
            return newGlobalParamConflictSemanticError(symbol->identifier->name, declaration_span,
                                                       symbol->identifier->declaration_span);
        }
    }
    // No bound set, we can set a global bound
    Bound *bound = newGlobalBound();
    Identifier *identifier = newIdentifier(symbol->value, type, bound, declaration_span);
    symbol->identifier = identifier;
    return NULL;
}
ParamType *newParamType(Type *type, struct HashEntry_t *symbol) {
    ParamType *param_type = (ParamType *) malloc(sizeof(ParamType));
    param_type->type = type;
    param_type->symbol = symbol;
    param_type->next = NULL;
    return param_type;
}
ParamTypeList *newParamTypeList() {
    ParamTypeList *list = (ParamTypeList *) malloc(sizeof(ParamTypeList));
    list->head = NULL;
    list->tail = NULL;
    list->arg_count = 0;
    return list;
}
void paramTypeListAppend(ParamTypeList *list, ParamType *param_type) {
    if (list->head == NULL) {
        list->head = param_type;
    } else {
        list->tail->next = param_type;
    }
    list->tail = param_type;
    list->arg_count++;
}
SemanticError *setParamBound(struct HashEntry_t *symbol, Type *type, struct HashEntry_t *func_symbol, Span *span) {
    if (symbol == NULL) {
        criticalError("Cannot set a param bound to a NULL symbol");
    }
    if (type == NULL) {
        criticalError("Cannot set a param bound to a NULL type");
    }
    if (symbol->identifier != NULL) {
        // The symbol already has a bound set
        // As we are setting a param bound, this could be an error
        // if the bound is a global bound

        if (symbol->identifier->bound->bound_type == BOUND_TYPE_GLOBAL) {
            // The symbol already has a global bound set
            // This is an error
            return newGlobalParamConflictSemanticError(symbol->identifier->name, span,
                                                       symbol->identifier->declaration_span);
        } else if (symbol->identifier->bound->bound_type == BOUND_TYPE_PARAM) {
            // The symbol already has a param bound set
            // This is only an error if the param is already declared in the same function

            IdentifierIterator *iterator = newIdentifierIterator(symbol->identifier);
            Identifier *last = NULL;
            while (!identifierIteratorDone(iterator)) {
                Identifier *identifier = getNextIdentifier(iterator);
                if (identifier->bound->symbol == func_symbol) {
                    // The param is already declared in the same function
                    // This is an error
                    destroyIdentifierIterator(iterator);
                    return newRedefIdentifierSemanticError(symbol->identifier->name, span,
                                                           identifier->declaration_span);
                }
                last = identifier;
            }
            destroyIdentifierIterator(iterator);
            if (last == NULL) {
                criticalError("Cannot find the last identifier of a param list");
            }
            // The param is not already declared in the same function
            // We can add it to the list
            last->next = newIdentifier(symbol->value, type, newParamBound(func_symbol), span);
            return NULL;
        }
    }
    // No bound set, we can set a param bound
    Bound *bound = newParamBound(func_symbol);
    Identifier *identifier = newIdentifier(symbol->value, type, bound, span);
    symbol->identifier = identifier;
    return NULL;
}
IdentifierIterator *newIdentifierIterator(Identifier *head) {
    IdentifierIterator *iterator = (IdentifierIterator *) malloc(sizeof(IdentifierIterator));
    iterator->next = head;
    return iterator;
}
Identifier *getNextIdentifier(IdentifierIterator *iterator) {
    Identifier *current = iterator->next;
    if (current != NULL) {
        iterator->next = current->next;
    }
    return current;
}
bool identifierIteratorDone(IdentifierIterator *iterator) {
    return iterator->next == NULL;
}
Type *newFunctionType(TypeBase returnType, ParamTypeList *param_list) {
    Type *type = (Type *) malloc(sizeof(Type));
    type->plural = TYPE_NATURE_FUNCTION;
    type->base = returnType;
    type->size = 1;
    type->arg_list = param_list;
    return type;
}
void destroyIdentifierIterator(IdentifierIterator *iterator) {
    free(iterator);
}
ParamIterator *newParamIterator(ParamTypeList *list) {
    ParamIterator *iterator = (ParamIterator *) malloc(sizeof(ParamIterator));
    iterator->next = list->head;
    return iterator;
}
ParamType *getNextParam(ParamIterator *iterator) {
    ParamType *current = iterator->next;
    if (current != NULL) {
        iterator->next = current->next;
    }
    return current;
}
bool paramIteratorDone(ParamIterator *iterator) {
    return iterator->next == NULL;
}
void destroyParamIterator(ParamIterator *iterator) {
    free(iterator);
}
bool paramListContains(ParamTypeList *list, struct HashEntry_t *symbol) {
    ParamIterator *iterator = newParamIterator(list);
    while (!paramIteratorDone(iterator)) {
        ParamType *param_type = getNextParam(iterator);
        if (param_type->symbol == symbol) {
            destroyParamIterator(iterator);
            return true;
        }
    }
    destroyParamIterator(iterator);
    return false;
}
bool isCompatible(TypeBase expected, TypeBase got) {
    switch (expected) {
        case TYPE_BASE_INT:
            return got == TYPE_BASE_INT || got == TYPE_BASE_CHAR;
        case TYPE_BASE_FLOAT:
            return got == TYPE_BASE_FLOAT;
        case TYPE_BASE_CHAR:
            return got == TYPE_BASE_CHAR || got == TYPE_BASE_INT;
        case TYPE_BASE_STRING:
            return got == TYPE_BASE_STRING;
        case TYPE_BASE_BOOL:
            return got == TYPE_BASE_BOOL;
        case TYPE_BASE_ERROR:
            return true;
    }
}
