//
// Created by vini84200 on 12/9/23.
//

#ifndef TYPES_H
#define TYPES_H
#include "semanticError.h"
#include "span.h"

typedef enum TypeNature_t {
    TYPE_NATURE_SCALAR,
    TYPE_NATURE_ARRAY,
    TYPE_NATURE_FUNCTION
} TypePlural;

typedef enum TypeBase_t {
    TYPE_BASE_INT,
    TYPE_BASE_FLOAT,
    TYPE_BASE_CHAR,
    TYPE_BASE_STRING,
    TYPE_BASE_BOOL,
    TYPE_BASE_ERROR
} TypeBase;

struct ast_node;
TypeBase getTypeBaseFromASTType(const struct ast_node *type);


typedef struct Type_t {
    TypePlural plural;
    TypeBase base;
    int size;
    struct ParamTypeList_t *arg_list;
} Type;

typedef struct ParamType_t {
    struct Type_t *type;
    struct HashEntry_t *symbol;
    struct ParamType_t *next;
} ParamType;

ParamType *newParamType(Type *type, struct HashEntry_t *symbol);

typedef struct ParamTypeList_t {
    ParamType *head;
    ParamType *tail;
    int arg_count;
} ParamTypeList;

ParamTypeList *newParamTypeList();
void paramTypeListAppend(ParamTypeList *list, ParamType *param_type);


Type *newScalarType(TypeBase base);
Type *newArrayType(TypeBase base, int size);

Type *newFunctionType(TypeBase returnType, ParamTypeList *param_list);


typedef enum BoundType_t {
    BOUND_TYPE_GLOBAL,
    BOUND_TYPE_PARAM,
} BoundType;

typedef struct Bound_t {
    BoundType bound_type;
    struct HashEntry_t *symbol;
} Bound;

Bound *newGlobalBound();
Bound *newParamBound(struct HashEntry_t *function_identifier);

typedef struct Identifier_t {
    char *name;
    Type *type;
    Bound *bound;
    Span *declaration_span;
    struct Identifier_t *next; // Used when the identifier is a parameter that can be declared multiple times
} Identifier;

Identifier *newIdentifier(char *name, Type *type, Bound *bound, Span *declaration_span);
SemanticError *setGlobalBound(struct HashEntry_t *symbol, Type *type, Span *declaration_span);
SemanticError *setParamBound(struct HashEntry_t *symbol, Type *type, struct HashEntry_t *func_symbol);

typedef struct IdentifierIterator_t {
    Identifier *next;
} IdentifierIterator;

IdentifierIterator *newIdentifierIterator(Identifier *head);
Identifier *getNextIdentifier(IdentifierIterator *iterator);
bool identifierIteratorDone(IdentifierIterator *iterator);
void destroyIdentifierIterator(IdentifierIterator *iterator);

typedef struct ParamIterator_t {
    ParamType *next;
} ParamIterator;

ParamIterator *newParamIterator(ParamTypeList *list);
ParamType *getNextParam(ParamIterator *iterator);
bool paramIteratorDone(ParamIterator *iterator);
void destroyParamIterator(ParamIterator *iterator);

bool paramListContains(ParamTypeList *list, struct HashEntry_t *symbol);

bool isCompatible(TypeBase expected, TypeBase got);



#endif//TYPES_H
