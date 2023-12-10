//
// Created by vini84200 on 12/9/23.
//

#include "semantics.h"


#include "errorHandler.h"
#include "lib.h"


#include <stdlib.h>

SemanticAnalyzer *analyzer;

void semanticTry(SemanticError *error) {
    SemanticError *current = error;
    while (current != NULL) {
        SemanticError *next = current->next;
        semanticErrorListAppend(analyzer->error_list, current);
        current = next;
    }
}

void semanticThrow(SemanticError *error) {
    semanticTry(error);
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
    analyzeImplementations(ast->children[1]);
}
void analyzeImplementations(struct ast_node *implList) {
    ASTListIterator *iterator = createASTListIterator(implList);
    while (!ASTIteratorDone(iterator)) {
        const AST *impl = getNextAST(iterator);
        if (impl->type == AST_IMPL_FUNC) {
            // FUNC_IMPL( sym=id, $0=cmd )
            if (impl->symbol->identifier == NULL) {
                // The function is not defined
                semanticThrow(newFunctionNotDeclaredSemanticError(impl->symbol->value, emptySpan()));
                continue;
            } else if (impl->symbol->identifier->type->plural != TYPE_NATURE_FUNCTION) {
                // The symbol is not a function
                semanticThrow(newIsNotAFunctionSemanticError(impl->symbol->value, emptySpan()));
                continue;
            }
            // The symbol is a function and it's defined
            // Can only be implemented once
            if (impl->symbol->implemented) {
                semanticThrow(newFunctionReimplementationSemanticError(impl->symbol->value, emptySpan(),
                                                                       impl->symbol->identifier->declaration_span));
                continue;
            }
            // Mark the function as implemented
            impl->symbol->implemented = true;

            Type type = *impl->symbol->identifier->type;
            analyzeCommand(impl->children[0], impl->symbol->identifier->type->arg_list, type);

        } else {
            criticalError("Invalid implementation type");
        }
    }
    destroyASTListIterator(iterator);

    // Check if all the functions are implemented
    SymbolIterator *symbol_iterator = createSymbolIterator(getSymbolTable(), SYMBOL_IDENTIFIER);
    while (!SymbolIteratorDone(symbol_iterator)) {
        const HashEntry *entry = getNextSymbol(symbol_iterator);
        if (entry->identifier == NULL) {
            // Already checked in the declarations, so we can skip it
            continue;
        } else {
            if (entry->identifier->type->plural == TYPE_NATURE_FUNCTION) {
                if (!entry->implemented) {
                    // The function is not implemented, this is an error
                    semanticThrow(newFunctionNotImplementedSemanticError(entry->value, emptySpan()));
                }
            }
        }
    }
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

SemanticAnalyzerResult *analyzeSemantics(AST *ast) {
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void analyzeCommand(struct ast_node *cmd, struct ParamTypeList_t *pList, Type funcType) {
    if (cmd->type == AST_EMPTY_COMMAND) {
        // Nothing to do
        return;
    }
    if (cmd->type == AST_COMMAND_BLOCK) {
        // BLOCK( $0=cmd_list )
        // Will have to analyze each command in the list
        ASTListIterator *iterator = createASTListIterator(cmd->children[0]);
        while (!ASTIteratorDone(iterator)) {

            const AST *cmd = getNextAST(iterator);
            analyzeCommand(cmd, pList, funcType);
        }
        destroyASTListIterator(iterator);
        return;
    }
    if (cmd->type == AST_COMMAND_ASSIGN) {
        // ASSIGN( sym=id, $0=expr )
        // Check if the identifier is defined in global scope
        if (cmd->symbol->identifier == NULL) {
            // The identifier is not defined
            // This has already been checked in the declarations, so we can skip it
            return;
        }
        if (cmd->symbol->identifier->bound->bound_type == BOUND_TYPE_PARAM) {
            // The identifier is a parameter
            // Check if the parameter is defined in the function
            if (paramListContains(pList, cmd->symbol->identifier->bound->symbol)) {
                // The parameter is defined in the function
                // This is an error as we can't assign to a parameter
                semanticThrow(newAssignToParamSemanticError(cmd->symbol->value, emptySpan()));
                return;
            }
            // The parameter is not defined in the function
            // This is an error
            semanticThrow(newUndefIdentifierSemanticError(cmd->symbol->value, emptySpan()));
            return;
        }
        if (cmd->symbol->identifier->bound->bound_type == BOUND_TYPE_GLOBAL) {
            // The identifier is a global variable
            TypeBase type = cmd->symbol->identifier->type->base;
            TypeBase exprType = getExpressionType(cmd->children[0], pList);
            if (!isCompatible(type, exprType) && exprType != TYPE_BASE_ERROR) {
                // The expression is not of the same type as the variable
                // This is an error
                semanticThrow(newAssignTypeMismatchSemanticError(cmd->symbol->value,
                                                                 type,
                                                                 exprType,
                                                                 emptySpan()));
                return;
            }

            switch (cmd->symbol->identifier->type->plural) {
                case TYPE_NATURE_ARRAY:
                    // The identifier is not a scalar
                    // This is an error
                    semanticThrow(newAssignToArraySemanticError(cmd->symbol->value, emptySpan()));
                    return;
                case TYPE_NATURE_FUNCTION:
                    // The identifier is not a scalar
                    // This is an error
                    semanticThrow(newAssignToFunctionSemanticError(cmd->symbol->value, emptySpan()));
                    return;
                case TYPE_NATURE_SCALAR:
                    return;
                default:
                    criticalError("Invalid type nature");
            }
        }
        criticalError("Invalid bound type");
    }
    if (cmd->type == AST_COMMAND_ASSIGN_ARRAY) {
        // ASSIGN_ARRAY( sym=id, $0=expr for index, $1=expr for value )
        if (cmd->symbol->identifier == NULL) {
            // The identifier is not defined
            semanticThrow(newUndefIdentifierSemanticError(cmd->symbol->value, emptySpan()));
            return;
        }
        if (cmd->symbol->identifier->bound->bound_type == BOUND_TYPE_PARAM) {
            // The identifier is a parameter
            // Check if the parameter is defined in the function
            if (paramListContains(pList, cmd->symbol->identifier->bound->symbol)) {
                // The parameter is defined in the function
                // This is an error as we can't assign to a parameter
                semanticThrow(newAssignToParamSemanticError(cmd->symbol->value, emptySpan()));
                return;
            }
            // The parameter is not defined in the function
            // This is an error
            semanticThrow(newUndefIdentifierSemanticError(cmd->symbol->value, emptySpan()));
            return;
        }
        if (cmd->symbol->identifier->bound->bound_type == BOUND_TYPE_GLOBAL) {
            // The identifier is a global variable
            TypeBase indexType = getExpressionType(cmd->children[0], pList);
            if (!isCompatible(TYPE_BASE_INT, indexType) && indexType != TYPE_BASE_ERROR) {
                // The index is not an integer expression
                // This is an error
                semanticThrow(newIndexScalarSemanticError(cmd->symbol->value, emptySpan()));
                return;
            }
            TypeBase exprType = getExpressionType(cmd->children[1], pList);
            TypeBase expectedType = cmd->symbol->identifier->type->base;
            if (!isCompatible(expectedType, exprType) && exprType != TYPE_BASE_ERROR) {
                // The expression is not of the same type as the variable
                // This is an error
                semanticThrow(newAssignTypeMismatchSemanticError(cmd->symbol->value,
                                                                 cmd->symbol->identifier->type->base,
                                                                 getExpressionType(cmd->children[1], pList),
                                                                 emptySpan()));
                return;
            }
            if (cmd->symbol->identifier->type->plural == TYPE_NATURE_ARRAY) {
                // The identifier is an array
                return;
            } else if (cmd->symbol->identifier->type->plural == TYPE_NATURE_FUNCTION) {
                // The identifier is not an array
                // This is an error
                semanticThrow(newIndexFunctionSemanticError(cmd->symbol->value, emptySpan()));
                return;
            } else if (cmd->symbol->identifier->type->plural == TYPE_NATURE_SCALAR) {
                // The identifier is not an array
                // This is an error
                semanticThrow(newIndexScalarSemanticError(cmd->symbol->value, emptySpan()));
                return;
            }
        }
        criticalError("Invalid bound type");
    }
    if (cmd->type == AST_COMMAND_PRINT_EXPR) {
        // PRINT_EXPR( $0=expr )
        TypeBase exprType = getExpressionType(cmd->children[0], pList);
        return;
    }
    if (cmd->type == AST_COMMAND_PRINT_STRING) {
        // PRINT_STRING( str=string )
        // Nothing to do
        return;
    }
    if (cmd->type == AST_COMMAND_IF) {
        // IF( $0=expr, $1=cmd, $2=cmd )
        TypeBase exprType = getExpressionType(cmd->children[0], pList);
        if (!isCompatible(TYPE_BASE_BOOL, exprType) && exprType != TYPE_BASE_ERROR) {
            // The expression is not a boolean expression
            // This is an error
            semanticThrow(newWrongOperandTypeSemanticErrorUnary("if", "boolean", exprType, emptySpan()));
            return;
        }
        analyzeCommand(cmd->children[1], pList, funcType);
        analyzeCommand(cmd->children[2], pList, funcType);
    }
    if (cmd->type == AST_COMMAND_WHILE) {
        // WHILE( $0=expr, $1=cmd )
        TypeBase exprType = getExpressionType(cmd->children[0], pList);
        if (!isCompatible(TYPE_BASE_BOOL, exprType) && exprType != TYPE_BASE_ERROR) {
            // The expression is not a boolean expression
            // This is an error
            semanticThrow(newWrongOperandTypeSemanticErrorUnary("while", "boolean", exprType, emptySpan()));
            return;
        }
        analyzeCommand(cmd->children[1], pList, funcType);
    }
    if (cmd->type == AST_COMMAND_RETURN) {
        // RETURN( $0=expr )
        TypeBase exprType = getExpressionType(cmd->children[0], pList);
        TypeBase expectedType = funcType.base;
        if (!isCompatible(expectedType, exprType) && exprType != TYPE_BASE_ERROR) {
            // The expression is not of the same type as the function
            // This is an error
            semanticThrow(newReturnWrongTypeSemanticError(expectedType, exprType, emptySpan()));
            return;
        }
    }
}


TypeBase getExpressionType(struct ast_node *expr, ParamTypeList *pList) {
    if (expr == NULL) {
        criticalError("Cannot get the type of a NULL expression");
    }
    switch (expr->type) {
        case AST_EXPR_IDENTIFIER:
            // VALUE( sym=id )
            if (expr->symbol->identifier == NULL) {
                // The identifier is not defined
                // This has already been checked in the declarations, so we can skip it
                return TYPE_BASE_ERROR;
            }
            switch (expr->symbol->identifier->bound->bound_type) {
                case BOUND_TYPE_GLOBAL:
                    // The identifier is a global variable
                    switch (expr->symbol->identifier->type->plural) {
                        case TYPE_NATURE_SCALAR:
                            // All good
                            break;
                        case TYPE_NATURE_ARRAY:
                            // This is an error
                            semanticThrow(newReadFromArraySemanticError(expr->symbol->value, emptySpan()));
                            return TYPE_BASE_ERROR;
                        case TYPE_NATURE_FUNCTION:
                            // This is an error
                            semanticThrow(newReadFromFunctionSemanticError(expr->symbol->value, emptySpan()));
                            return TYPE_BASE_ERROR;
                    }
                    return expr->symbol->identifier->type->base;
                case BOUND_TYPE_PARAM:
                    // The identifier is a parameter
                    // Check if the parameter is defined in the function
                    if (paramListContains(pList, expr->symbol->identifier->bound->symbol)) {
                        // The parameter is defined in the function
                        // This is ok
                        // Parameters are always scalars
                        return expr->symbol->identifier->type->base;
                    }
                    // The parameter is not defined in the function
                    // This is an error
                    semanticThrow(newUndefIdentifierSemanticError(expr->symbol->value, emptySpan()));
                    return TYPE_BASE_ERROR;
            }
            break;
        case AST_EXPR_ARRAY_GET:
            // ARRAY_GET( sym=id, $0=expr )
            if (expr->symbol->identifier == NULL) {
                // The identifier is not defined
                // This has already been checked in the declarations, so we can skip it
                return TYPE_BASE_ERROR;
            }
            switch (expr->symbol->identifier->bound->bound_type) {
                case BOUND_TYPE_GLOBAL:
                    // The identifier is a global variable
                    switch (expr->symbol->identifier->type->plural) {
                        case TYPE_NATURE_SCALAR:
                            // This is an error
                            semanticThrow(newIndexScalarSemanticError(expr->symbol->value, emptySpan()));
                            return TYPE_BASE_ERROR;
                        case TYPE_NATURE_ARRAY: {
                            // This is ok
                            TypeBase indexType = getExpressionType(expr->children[0], pList);
                            if (indexType != TYPE_BASE_INT && indexType != TYPE_BASE_ERROR) {
                                // The index is not an integer expression
                                // This is an error
                                semanticThrow(newIndexNotIntSemanticError(expr->symbol->value, indexType, emptySpan()));
                                return TYPE_BASE_ERROR;
                            }
                            return expr->symbol->identifier->type->base;
                        }
                        case TYPE_NATURE_FUNCTION:
                            // This is an error
                            semanticThrow(newIndexFunctionSemanticError(expr->symbol->value, emptySpan()));
                            return TYPE_BASE_ERROR;
                    }
                case BOUND_TYPE_PARAM:
                    // The identifier is a parameter
                    // Check if the parameter is defined in the function
                    if (paramListContains(pList, expr->symbol->identifier->bound->symbol)) {
                        // The parameter is defined in the function
                        // This is an error as arrays are not allowed as parameters
                        semanticThrow(newIndexScalarSemanticError(expr->symbol->value, emptySpan()));
                        return TYPE_BASE_ERROR;
                    }
                    // The parameter is not defined in the function
                    // This is an error
                    semanticThrow(newUndefIdentifierSemanticError(expr->symbol->value, emptySpan()));
                    return TYPE_BASE_ERROR;
            }
            break;
        case AST_EXPR_FUNC_CALL:
            if (expr->symbol->identifier == NULL) {
                // The identifier is not defined
                // This has already been checked in the declarations, so we can skip it
                return TYPE_BASE_ERROR;
            }
            switch (expr->symbol->identifier->bound->bound_type) {
                case BOUND_TYPE_GLOBAL:
                    // The identifier is a global variable
                    switch (expr->symbol->identifier->type->plural) {
                        case TYPE_NATURE_SCALAR:
                            // This is an error
                            semanticThrow(newCallScalarSemanticError(expr->symbol->value, emptySpan()));
                            return TYPE_BASE_ERROR;
                        case TYPE_NATURE_ARRAY:
                            // This is an error
                            semanticThrow(newCallArraySemanticError(expr->symbol->value, emptySpan()));
                            return TYPE_BASE_ERROR;
                        case TYPE_NATURE_FUNCTION:
                            // This is ok
                            // TODO: Check the arguments
                            return expr->symbol->identifier->type->base;
                    }
                case BOUND_TYPE_PARAM:
                    // The identifier is a parameter
                    // Check if the parameter is defined in the function
                    if (paramListContains(pList, expr->symbol->identifier->bound->symbol)) {
                        // The parameter is defined in the function
                        // This is an error as functions are not allowed as parameters
                        semanticThrow(newCallScalarSemanticError(expr->symbol->value, emptySpan()));
                        return TYPE_BASE_ERROR;
                    }
                    // The parameter is not defined in the function
                    // This is an error
                    semanticThrow(newUndefIdentifierSemanticError(expr->symbol->value, emptySpan()));
                    return TYPE_BASE_ERROR;
            }
        case AST_EXPR_LIT_INT:
            // LIT_INT( val=int )
            return TYPE_BASE_INT;
        case AST_EXPR_LIT_FLOAT:
            // LIT_FLOAT( val=float )
            return TYPE_BASE_FLOAT;
        case AST_EXPR_LIT_CHAR:
            // LIT_CHAR( val=char )
            return TYPE_BASE_CHAR;
        // Binary operations that receive numeric operands and return a numeric value
        case AST_EXPR_ADD:
        case AST_EXPR_SUB:
        case AST_EXPR_MUL:
        case AST_EXPR_DIV: {
            // ADD( $0=expr, $1=expr )
            // SUB( $0=expr, $1=expr )
            // MUL( $0=expr, $1=expr )
            char *op;
            switch (expr->type) {
                case AST_EXPR_ADD:
                    op = "+";
                    break;
                case AST_EXPR_SUB:
                    op = "-";
                    break;
                case AST_EXPR_MUL:
                    op = "*";
                    break;
                case AST_EXPR_DIV:
                    op = "/";
                    break;
                default:
                    criticalError("Invalid binary operation");
            }
            TypeBase type0 = getExpressionType(expr->children[0], pList);
            TypeBase type1 = getExpressionType(expr->children[1], pList);
            bool big = false;
            switch (type0) {
                case TYPE_BASE_INT:
                    big = true;
                case TYPE_BASE_CHAR:
                    if (type1 == TYPE_BASE_INT || type1 == TYPE_BASE_CHAR) {
                        return big ? TYPE_BASE_INT : type1;
                    } else if (type1 != TYPE_BASE_ERROR) {
                        semanticThrow(newWrongOperandTypeSemanticErrorBinaryRight(op, "int/char", type1, emptySpan()));
                        return TYPE_BASE_ERROR;
                    } else {
                        return TYPE_BASE_ERROR;
                    }
                case TYPE_BASE_FLOAT:
                    if (type1 == TYPE_BASE_FLOAT) {
                        return TYPE_BASE_FLOAT;
                    } else if (type1 != TYPE_BASE_ERROR) {
                        semanticThrow(newWrongOperandTypeSemanticErrorBinaryRight(op, "float", type1, emptySpan()));
                        return TYPE_BASE_ERROR;
                    } else {
                        return TYPE_BASE_ERROR;
                    }
                case TYPE_BASE_BOOL:
                    semanticThrow(newWrongOperandTypeSemanticErrorBinaryLeft(op, "numeric", type0, emptySpan()));
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_ERROR:
                    // An error has already been thrown
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_STRING:
                    criticalError("Strings are not supported");
            }
        }
            // Binary operations that receive numeric operands and return a boolean value
        case AST_EXPR_EQ:
        case AST_EXPR_NE:
        case AST_EXPR_LESS:
        case AST_EXPR_GREATER:
        case AST_EXPR_LE:
        case AST_EXPR_GE: {
            // EQ( $0=expr, $1=expr )
            // NE( $0=expr, $1=expr )
            // LESS( $0=expr, $1=expr )
            // GREATER( $0=expr, $1=expr )
            // LE( $0=expr, $1=expr )
            // GE( $0=expr, $1=expr )
            char *op;
            switch (expr->type) {
                case AST_EXPR_EQ:
                    op = "==";
                    break;
                case AST_EXPR_NE:
                    op = "!=";
                    break;
                case AST_EXPR_LESS:
                    op = "<";
                    break;
                case AST_EXPR_GREATER:
                    op = ">";
                    break;
                case AST_EXPR_LE:
                    op = "<=";
                    break;
                case AST_EXPR_GE:
                    op = ">=";
                    break;
                default:
                    criticalError("Invalid binary operation");
            }
            TypeBase type0 = getExpressionType(expr->children[0], pList);
            TypeBase type1 = getExpressionType(expr->children[1], pList);
            bool big = false;
            switch (type0) {
                case TYPE_BASE_INT:
                case TYPE_BASE_CHAR:
                    if (type1 == TYPE_BASE_INT || type1 == TYPE_BASE_CHAR) {
                        return TYPE_BASE_BOOL;
                    } else if (type1 != TYPE_BASE_ERROR) {
                        semanticThrow(newWrongOperandTypeSemanticErrorBinaryRight(op, "int/char", type1, emptySpan()));
                        return TYPE_BASE_ERROR;
                    } else {
                        return TYPE_BASE_ERROR;
                    }
                case TYPE_BASE_FLOAT:
                    if (type1 == TYPE_BASE_FLOAT) {
                        return TYPE_BASE_BOOL;
                    } else if (type1 != TYPE_BASE_ERROR) {
                        semanticThrow(newWrongOperandTypeSemanticErrorBinaryRight(op, "float", type1, emptySpan()));
                        return TYPE_BASE_ERROR;
                    } else {
                        return TYPE_BASE_ERROR;
                    }
                case TYPE_BASE_BOOL:
                    semanticThrow(newWrongOperandTypeSemanticErrorBinaryLeft(op, "numeric", type0, emptySpan()));
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_ERROR:
                    // An error has already been thrown
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_STRING:
                    criticalError("Strings are not supported");
            }
        }
            // Binary operations that receive boolean operands and return a boolean value
        case AST_EXPR_AND:
        case AST_EXPR_OR: {
            // AND( $0=expr, $1=expr )
            // OR( $0=expr, $1=expr )
            char *op;
            switch (expr->type) {
                case AST_EXPR_AND:
                    op = "&";
                    break;
                case AST_EXPR_OR:
                    op = "|";
                    break;
                default:
                    criticalError("Invalid binary operation");
            }
            TypeBase type0 = getExpressionType(expr->children[0], pList);
            TypeBase type1 = getExpressionType(expr->children[1], pList);
            switch (type0) {
                case TYPE_BASE_INT:
                case TYPE_BASE_CHAR:
                case TYPE_BASE_FLOAT:
                    semanticThrow(newWrongOperandTypeSemanticErrorBinaryLeft(op, "boolean", type0, emptySpan()));
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_BOOL:
                    if (type1 == TYPE_BASE_BOOL) {
                        return TYPE_BASE_BOOL;
                    } else if (type1 != TYPE_BASE_ERROR) {
                        semanticThrow(newWrongOperandTypeSemanticErrorBinaryRight(op, "boolean", type1, emptySpan()));
                        return TYPE_BASE_ERROR;
                    } else {
                        return TYPE_BASE_ERROR;
                    }
                case TYPE_BASE_ERROR:
                    // An error has already been thrown
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_STRING:
                    criticalError("Strings are not supported");
            }
        }
        // Unary operations that receive numeric operands and return a numeric value
        case AST_EXPR_MINUS: {
            // MINUS( $0=expr )
            TypeBase type0 = getExpressionType(expr->children[0], pList);
            switch (type0) {
                case TYPE_BASE_INT:
                    return TYPE_BASE_INT;
                case TYPE_BASE_CHAR:
                    return TYPE_BASE_CHAR;
                case TYPE_BASE_FLOAT:
                    return TYPE_BASE_FLOAT;
                case TYPE_BASE_BOOL:
                    semanticThrow(newWrongOperandTypeSemanticErrorUnary("-", "numeric", type0, emptySpan()));
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_ERROR:
                    // An error has already been thrown
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_STRING:
                    criticalError("Strings are not supported");
            }
        }
        // Unary operations that receive boolean operands and return a boolean value
        case AST_EXPR_NOT: {
            // NOT( $0=expr )
            TypeBase type0 = getExpressionType(expr->children[0], pList);
            switch (type0) {
                case TYPE_BASE_INT:
                case TYPE_BASE_CHAR:
                case TYPE_BASE_FLOAT:
                    semanticThrow(newWrongOperandTypeSemanticErrorUnary("~", "boolean", type0, emptySpan()));
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_BOOL:
                    return TYPE_BASE_BOOL;
                case TYPE_BASE_ERROR:
                    // An error has already been thrown
                    return TYPE_BASE_ERROR;
                case TYPE_BASE_STRING:
                    criticalError("Strings are not supported");
            }
        }
        case AST_EXPR_READ:
            // READ( $0=type )
            // The type is the declared type in the read command
            return getTypeBaseFromASTType(expr->children[0]);
    }
}
