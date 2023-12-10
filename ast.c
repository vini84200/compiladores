//
// Created by vini84200 on 11/30/23.
//

#include "ast.h"


#include "errorHandler.h"
#include "lib.h"


#include <stdio.h>
#include <stdlib.h>

FILE *output_file;

void open_output_file(char *filename) {
    output_file = fopen(filename, "w");
    if (output_file == NULL) {
        fprintf(stderr, "Cannot open output file %s\n", filename);
        exit(1);
    }
}


AST *createASTnoSpan(int type, HashEntry *symbol, AST *child0, AST *child1, AST *child2, AST *child3) {
    AST *node = (AST *) calloc(1, sizeof(AST));
    node->type = type;
    node->symbol = symbol;
    node->children[0] = child0;
    node->children[1] = child1;
    node->children[2] = child2;
    node->children[3] = child3;
    node->span = emptySpan();
    return node;
}
char *printASTType(int type) {
    switch (type) {
        case AST_TYPE_INT:
            return "int";
        case AST_TYPE_FLOAT:
            return "float";
        case AST_TYPE_CHAR:
            return "char";
        default:
            return "UNK";
    }
}

void printIdent(int count, char ident) {
    for (int i = 0; i < count; i++) {
        printf("%c", ident);
    }
}

void printAST(AST *node, int level) {
    switch (node->type) {
        case AST_PROGRAM:
            fprintf(output_file, "/// PROGRAM \\\\\\\n");
            fprintf(output_file, "// DEFINITIONS\n");
            if (node->children[0] != NULL) {
                printAST(node->children[0], level);
            }
            fprintf(output_file, "// CODE\n");
            if (node->children[1] != NULL) {
                printAST(node->children[1], level);
            }

            break;

        case AST_VAR_DECLARATION:
            printIdent(level * 2, ' ');

            fprintf(output_file, "%s %s = %s;\n",
                    printASTType(node->children[0]->type),
                    node->symbol->value,
                    node->children[1]->symbol->value);
            break;
        case AST_ARRAY_DECLARATION:
            printIdent(level * 2, ' ');
            if (node->children[2] != NULL) {
                fprintf(output_file, "%s %s[%s] \n",
                        printASTType(node->children[0]->type),
                        node->symbol->value,
                        node->children[1]->symbol->value);
                printAST(node->children[2], level + 1);
                fprintf(output_file, ";\n");
            } else {
                fprintf(output_file, "%s %s[%s];\n",
                        printASTType(node->children[0]->type),
                        node->symbol->value,
                        node->children[1]->symbol->value);
            }
            break;

        case AST_ARRAY_VALUES:
            printIdent(level * 2, ' ');

            fprintf(output_file, "%s\n", node->children[0]->symbol->value);
            if (node->children[1] != NULL) {
                printAST(node->children[1], level);
            }
            break;

        case AST_TYPE_INT:

            printIdent(level * 2, ' ');
            fprintf(output_file, "INT\n");
            break;
        case AST_DECLARATION_LIST:
            if (node->children[0] != NULL) {
                printAST(node->children[0], level);
            }
            if (node->children[1] != NULL) {
                printAST(node->children[1], level);
            }
            break;
        case AST_CODE_LIST:
            if (node->children[0] != NULL) {
                printAST(node->children[0], level);
            }
            if (node->children[1] != NULL) {
                printAST(node->children[1], level);
            }
            break;
        case AST_FUNC_DECLARATION:
            printIdent(level * 2, ' ');
            fprintf(output_file, "%s %s(\n", printASTType(node->children[0]->type), node->symbol->value);
            if (node->children[1] != NULL) {
                // Param list
                printAST(node->children[1], level + 1);
            }
            fprintf(output_file, ");\n");

            break;
        case AST_PARAM_LIST:
            printAST(node->children[0], level);
            if (node->children[1] != NULL) {
                printAST(node->children[1], level);
            }
            break;
        case AST_PARAM:
            printIdent(level * 2, ' ');
            fprintf(output_file, "%s %s,\n", printASTType(node->children[0]->type), node->symbol->value);
            break;
        case AST_IMPL_FUNC:
            printIdent(level * 2, ' ');
            fprintf(output_file, "code %s \n", node->symbol->value);
            printAST(node->children[0], level + 1);
            break;
        case AST_COMMAND_BLOCK:
            printIdent(level * 2, ' ');
            fprintf(output_file, "{\n");
            if (node->children[0] != NULL) {
                printAST(node->children[0], level + 1);
            }
            printIdent(level * 2, ' ');
            fprintf(output_file, "}\n");
            break;
        case AST_COMMAND_LIST:
            printAST(node->children[0], level);
            if (node->children[1] != NULL) {
                printAST(node->children[1], level);
            }
            break;
        case AST_COMMAND_ASSIGN_ARRAY:
            printIdent(level * 2, ' ');
            fprintf(output_file, "%s[(\n", node->symbol->value);
            printAST(node->children[0], level + 1);
            printIdent(level * 2, ' ');
            fprintf(output_file, ")] = (\n");
            printIdent(level * 2 + 2, ' ');
            printAST(node->children[1], level + 1);
            printIdent(level * 2, ' ');
            fprintf(output_file, ");\n");
            break;
        case AST_COMMAND_IF:
            printIdent(level * 2, ' ');
            fprintf(output_file, "if(\n");
            printIdent(level * 2 + 2, ' ');
            printAST(node->children[0], level + 1);
            printIdent(level * 2, ' ');
            fprintf(output_file, ")\n");
            printAST(node->children[1], level + 1);
            printIdent(level * 2, ' ');
            fprintf(output_file, "else\n");
            printAST(node->children[2], level + 1);
            break;
        case AST_COMMAND_WHILE:
            printIdent(level * 2, ' ');
            fprintf(output_file, "while(\n");
            printAST(node->children[0], level + 1);
            printIdent(level * 2, ' ');
            fprintf(output_file, ")\n");
            printAST(node->children[1], level + 1);
            break;
        case AST_COMMAND_ASSIGN:
            printIdent(level * 2, ' ');
            fprintf(output_file, "%s = (\n", node->symbol->value);
            printIdent(level * 2 + 2, ' ');
            printAST(node->children[0], level + 1);
            fprintf(output_file, "\n");
            printIdent(level * 2, ' ');
            fprintf(output_file, ");\n");
            break;
        case AST_EMPTY_COMMAND:
            printIdent(level * 2, ' ');
            fprintf(output_file, "/// EMPTY COMMAND \\\\\\;\n");
            break;
        case AST_COMMAND_PRINT_EXPR:
            printIdent(level * 2, ' ');
            fprintf(output_file, "print (\n");
            printIdent(level * 2 + 2, ' ');
            printAST(node->children[0], level + 1);
            printIdent(level * 2, ' ');
            fprintf(output_file, "\n);\n");
            break;
        case AST_COMMAND_PRINT_STRING:
            printIdent(level * 2, ' ');
            fprintf(output_file, "print %s;\n", node->symbol->value);
            break;
        case AST_EXPR_IDENTIFIER:
            if (node->symbol != NULL) {
                fprintf(output_file, "%s", node->symbol->value);
            } else {
                fprintf(output_file, "NULL\n");
            }
            break;
        case AST_EXPR_LIT_INT:
        case AST_EXPR_LIT_CHAR:
        case AST_EXPR_LIT_FLOAT:
            fprintf(output_file, "%s", node->symbol->value);
            break;
        case AST_EXPR_ARRAY_GET:
            fprintf(output_file, "%s[(", node->symbol->value);
            printAST(node->children[0], level + 1);
            fprintf(output_file, ")]");
            break;
        case AST_EXPR_ADD:
            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);
            fprintf(output_file, ")  +  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");
            break;
        case AST_EXPR_SUB:
            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);
            fprintf(output_file, ")  -  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");
            break;
        case AST_EXPR_MUL:
            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);
            fprintf(output_file, ")  *  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");
            break;
        case AST_EXPR_DIV:
            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);
            fprintf(output_file, ")  /  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");
            break;
        case AST_EXPR_LESS:
            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);
            fprintf(output_file, ") <  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");
            break;
        case AST_EXPR_GREATER:
            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);

            fprintf(output_file, ") >  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");

            break;
        case AST_EXPR_LE:

            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);

            fprintf(output_file, ") <=  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");

            break;

        case AST_EXPR_GE:

            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);

            fprintf(output_file, ") >=  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");

            break;

        case AST_EXPR_EQ:

            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);

            fprintf(output_file, ") ==  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");

            break;

        case AST_EXPR_NE:

            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);

            fprintf(output_file, ") !=  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");

            break;
        case AST_EXPR_AND:

            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);

            fprintf(output_file, ") &  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");

            break;

        case AST_EXPR_OR:

            fprintf(output_file, "(");
            printAST(node->children[0], level + 1);

            fprintf(output_file, ") |  (");
            printAST(node->children[1], level + 1);
            fprintf(output_file, ")");

            break;
        case AST_EXPR_NOT:

            fprintf(output_file, "~(");
            printAST(node->children[0], level + 1);
            fprintf(output_file, ")");

            break;
        case AST_EXPR_MINUS:

            fprintf(output_file, "-(");
            printAST(node->children[0], level + 1);
            fprintf(output_file, ")");

            break;
        case AST_EXPR_READ:
            fprintf(output_file, "input (%s)", printASTType(node->children[0]->type));

            break;
        case AST_EXPR_FUNC_CALL:
            fprintf(output_file, "%s(\n", node->symbol->value);
            if (node->children[0] != NULL) {
                printAST(node->children[0], level + 1);
            }
            printIdent(level * 2, ' ');
            fprintf(output_file, ")\n");
            break;
        case AST_EXPR_LIST:
            printIdent(level * 2, ' ');
            fprintf(output_file, "(\n");

            printAST(node->children[0], level);
            printIdent(level * 2, ' ');
            fprintf(output_file, ")\n");

            if (node->children[1] != NULL) {
                printIdent(level * 2, ' ');
                fprintf(output_file, ",\n");
                printAST(node->children[1], level);
            }
            break;
        case AST_COMMAND_RETURN:
            printIdent(level * 2, ' ');
            fprintf(output_file, "return (\n");
            printAST(node->children[0], level + 1);
            printIdent(level * 2, ' ');
            fprintf(output_file, ");\n");
            break;

        default:
            printIdent(level * 2, ' ');
            fprintf(output_file, "Type: %d\n", node->type);
            if (node->symbol != NULL) {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Symbol: %s\n", node->symbol->value);
            }
            if (node->children[0] != NULL) {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Child 0\n");
                printAST(node->children[0], level + 1);
            } else {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Child 0: NULL\n");
                break;
            }
            if (node->children[1] != NULL) {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Child 1\n");
                printAST(node->children[1], level + 1);

            } else {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Child 1: NULL\n");
                break;
            }
            if (node->children[2] != NULL) {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Child 2\n");
                printAST(node->children[2], level + 1);
            } else {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Child 2: NULL\n");
                break;
            }
            if (node->children[3] != NULL) {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Child 3\n");
                printAST(node->children[3], level + 1);
            } else {
                printIdent(level * 2, ' ');
                fprintf(output_file, "Child 3: NULL\n");
                break;
            }

            break;
    }
}
AST *createASTProgram(AST *declaration_list, AST *code_list, struct YYLTYPE loc) {
    return createAST(AST_PROGRAM, NULL, declaration_list, code_list, NULL, NULL, loc);
}
AST *createASTDeclList(AST *declaration, AST *declaration_list, struct YYLTYPE loc) {
    return createAST(AST_DECLARATION_LIST, NULL, declaration, declaration_list, NULL, NULL, loc);
}
AST *createASTVarDecl(AST *type, HashEntry *identifier, AST *value, struct YYLTYPE loc) {
    return createAST(AST_VAR_DECLARATION, identifier, type, value, NULL, NULL, loc);
}
AST *createASTType(int type, struct YYLTYPE loc) {
    return createAST(AST_TYPE_BASE + type, NULL, NULL, NULL, NULL, NULL, loc);
}
AST *createASTValue(HashEntry *value, struct YYLTYPE loc) {
    return createAST(AST_VALUE, value, NULL, NULL, NULL, NULL, loc);
}
AST *createASTArrayDecl(AST *type, HashEntry *identifier, HashEntry *size, AST *value, struct YYLTYPE loc) {
    AST *array_size = createASTValue(size, loc);
    return createAST(AST_ARRAY_DECLARATION, identifier, type, array_size, value, NULL, loc);
}
AST *createASTArrayValues(AST *value, AST *values, struct YYLTYPE loc) {
    return createAST(AST_ARRAY_VALUES, NULL, value, values, NULL, NULL, loc);
}
AST *createASTFuncDecl(AST *type, HashEntry *identifier, AST *param_list, struct YYLTYPE loc) {
    return createAST(AST_FUNC_DECLARATION, identifier, type, param_list, NULL, NULL, loc);
}
AST *createASTParamList(AST *param, AST *param_list, struct YYLTYPE loc) {
    return createAST(AST_PARAM_LIST, NULL, param, param_list, NULL, NULL, loc);
}
AST *createASTParam(AST *type, HashEntry *identifier, struct YYLTYPE loc) {
    return createAST(AST_PARAM, identifier, type, NULL, NULL, NULL, loc);
}
AST *createASTCodeList(AST *code, AST *code_list, struct YYLTYPE loc) {
    return createAST(AST_CODE_LIST, NULL, code, code_list, NULL, NULL, loc);
}

AST *createASTImplFunc(HashEntry *identifier, AST *command, struct YYLTYPE loc) {
    return createAST(AST_IMPL_FUNC, identifier, command, NULL, NULL, NULL, loc);
}
AST *createASTEmptyCommand(struct YYLTYPE loc) {
    return createAST(AST_EMPTY_COMMAND, NULL, NULL, NULL, NULL, NULL, loc);
}
AST *createASTCommandBlock(AST *command_list, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_BLOCK, NULL, command_list, NULL, NULL, NULL, loc);
}
AST *createASTCommandList(AST *command, AST *command_list, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_LIST, NULL, command, command_list, NULL, NULL, loc);
}
AST *createASTCommandAssign(HashEntry *identifier, AST *expr, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_ASSIGN, identifier, expr, NULL, NULL, NULL, loc);
}
AST *createASTCommandAssignArray(HashEntry *identifier, AST *expr_index, AST *expr_value, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_ASSIGN_ARRAY, identifier, expr_index, expr_value, NULL, NULL, loc);
}

AST *createASTCommandPrintExpr(AST *expr, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_PRINT_EXPR, NULL, expr, NULL, NULL, NULL, loc);
}
AST *createASTCommandPrintString(HashEntry *string, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_PRINT_STRING, string, NULL, NULL, NULL, NULL, loc);
}
AST *createASTCommandIf(AST *expr, AST *command_if, AST *command_else, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_IF, NULL, expr, command_if, command_else, NULL, loc);
}
AST *createASTCommandWhile(AST *expr, AST *command, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_WHILE, NULL, expr, command, NULL, NULL, loc);
}
AST *createASTExprVar(HashEntry *identifier, struct YYLTYPE loc) {
    return createAST(AST_EXPR_IDENTIFIER, identifier, NULL, NULL, NULL, NULL, loc);
}
AST *createASTExprArrayGet(HashEntry *identifier, AST *expr_index, struct YYLTYPE loc) {
    return createAST(AST_EXPR_ARRAY_GET, identifier, expr_index, NULL, NULL, NULL, loc);
}
AST *createASTExprFuncCall(HashEntry *identifier, AST *expr_list, struct YYLTYPE loc) {
    return createAST(AST_EXPR_FUNC_CALL, identifier, expr_list, NULL, NULL, NULL, loc);
}
AST *createASTExprList(AST *expr, AST *expr_list, struct YYLTYPE loc) {
    return createAST(AST_EXPR_LIST, NULL, expr, expr_list, NULL, NULL, loc);
}
AST *createASTExprLitInt(HashEntry *value, struct YYLTYPE loc) {
    return createAST(AST_EXPR_LIT_INT, value, NULL, NULL, NULL, NULL, loc);
}
AST *createASTExprLitFloat(HashEntry *value, struct YYLTYPE loc) {
    return createAST(AST_EXPR_LIT_FLOAT, value, NULL, NULL, NULL, NULL, loc);
}
AST *createASTExprLitChar(HashEntry *value, struct YYLTYPE loc) {
    return createAST(AST_EXPR_LIT_CHAR, value, NULL, NULL, NULL, NULL, loc);
}
AST *createASTExprAdd(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_ADD, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprSub(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_SUB, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprMul(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_MUL, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprDiv(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_DIV, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprLess(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_LESS, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprGreater(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_GREATER, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprLE(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_LE, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprGE(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_GE, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprEQ(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_EQ, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprNE(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_NE, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprAnd(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_AND, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprOr(AST *expr0, AST *expr1, struct YYLTYPE loc) {
    return createAST(AST_EXPR_OR, NULL, expr0, expr1, NULL, NULL, loc);
}
AST *createASTExprNot(AST *expr, struct YYLTYPE loc) {
    return createAST(AST_EXPR_NOT, NULL, expr, NULL, NULL, NULL, loc);
}
AST *createASTExprMinus(AST *expr, struct YYLTYPE loc) {
    return createAST(AST_EXPR_MINUS, NULL, expr, NULL, NULL, NULL, loc);
}
AST *createASTExprRead(AST *type, struct YYLTYPE loc) {
    return createAST(AST_EXPR_READ, NULL, type, NULL, NULL, NULL, loc);
}
AST *createASTCommandReturn(AST *expr, struct YYLTYPE loc) {
    return createAST(AST_COMMAND_RETURN, NULL, expr, NULL, NULL, NULL, loc);
}
ASTListIterator *createASTListIterator(AST *list) {
    ASTListIterator *iterator = (ASTListIterator *) calloc(1, sizeof(ASTListIterator));
    iterator->next = list;
    iterator->index = 0;
    return iterator;
}

AST *getNextAST(ASTListIterator *iterator) {
    AST *current = iterator->next;
    if (current != NULL) {
        iterator->next = current->children[1];
        iterator->index++;
        return current->children[0];
    } else {
        return NULL;
    }
}
bool ASTIteratorDone(ASTListIterator *iterator) {
    return iterator->next == NULL;
}
void destroyASTListIterator(ASTListIterator *iterator) {
    free(iterator);
}
void destroyAST(AST *node) {

    if (node->children[0] != NULL) {
        destroyAST(node->children[0]);
    }
    if (node->children[1] != NULL) {
        destroyAST(node->children[1]);
    }
    if (node->children[2] != NULL) {
        destroyAST(node->children[2]);
    }
    if (node->children[3] != NULL) {
        destroyAST(node->children[3]);
    }
    free(node);

}
AST *createAST(int type, HashEntry *symbol, AST *child0, AST *child1, AST *child2, AST *child3, struct YYLTYPE location) {
    AST *node = (AST *) calloc(1, sizeof(AST));
    node->type = type;
    node->symbol = symbol;
    node->children[0] = child0;
    node->children[1] = child1;
    node->children[2] = child2;
    node->children[3] = child3;
    node->span = spanFromLocation(&location);
    return node;
}

void astPrintNodeTypes(AST *node) {
#define PRINT_TYPE(type) case type: printf(#type); break;
    switch (node->type) {
        PRINT_TYPE(AST_PROGRAM)
        PRINT_TYPE(AST_DECLARATION_LIST)
        PRINT_TYPE(AST_VAR_DECLARATION)
        PRINT_TYPE(AST_FUNC_DECLARATION)
        PRINT_TYPE(AST_ARRAY_DECLARATION)
        PRINT_TYPE(AST_ARRAY_VALUES)
        PRINT_TYPE(AST_PARAM_LIST)
        PRINT_TYPE(AST_PARAM)
        PRINT_TYPE(AST_TYPE_INT)
        PRINT_TYPE(AST_TYPE_FLOAT)
        PRINT_TYPE(AST_TYPE_CHAR)
        PRINT_TYPE(AST_VALUE)
        PRINT_TYPE(AST_CODE_LIST)
        PRINT_TYPE(AST_IMPL_FUNC)
        PRINT_TYPE(AST_EMPTY_COMMAND)
        PRINT_TYPE(AST_COMMAND_BLOCK)
        PRINT_TYPE(AST_COMMAND_LIST)
        PRINT_TYPE(AST_COMMAND_ASSIGN)
        PRINT_TYPE(AST_COMMAND_ASSIGN_ARRAY)
        PRINT_TYPE(AST_COMMAND_PRINT_EXPR)
        PRINT_TYPE(AST_COMMAND_PRINT_STRING)
        PRINT_TYPE(AST_COMMAND_IF)
        PRINT_TYPE(AST_COMMAND_WHILE)
        PRINT_TYPE(AST_EXPR_IDENTIFIER)
        PRINT_TYPE(AST_EXPR_ARRAY_GET)
        PRINT_TYPE(AST_EXPR_FUNC_CALL)
        PRINT_TYPE(AST_EXPR_LIST)
        PRINT_TYPE(AST_EXPR_LIT_INT)
        PRINT_TYPE(AST_EXPR_LIT_FLOAT)
        PRINT_TYPE(AST_EXPR_LIT_CHAR)
        PRINT_TYPE(AST_EXPR_ADD)
        PRINT_TYPE(AST_EXPR_SUB)
        PRINT_TYPE(AST_EXPR_MUL)
        PRINT_TYPE(AST_EXPR_DIV)
        PRINT_TYPE(AST_EXPR_LESS)
        PRINT_TYPE(AST_EXPR_GREATER)
        PRINT_TYPE(AST_EXPR_LE)
        PRINT_TYPE(AST_EXPR_GE)
        PRINT_TYPE(AST_EXPR_EQ)
        PRINT_TYPE(AST_EXPR_NE)
        PRINT_TYPE(AST_EXPR_AND)
        PRINT_TYPE(AST_EXPR_OR)
        PRINT_TYPE(AST_EXPR_NOT)
        PRINT_TYPE(AST_EXPR_MINUS)
        PRINT_TYPE(AST_EXPR_READ)
        PRINT_TYPE(AST_COMMAND_RETURN)
    }
}

void astPrintDebug(AST *node, int level) {
    if (node == NULL) {
        return;
    }
    printIdent(level * 2, ' ');
    printf("(");
    astPrintNodeTypes(node);

    if (node->symbol != NULL) {
        printIdent(level * 2, ' ');
        printf(" sym: '%s'", node->symbol->value);
    }

    printf(" (%d, %d) - (%d, %d) \n", node->span->line, node->span->collumn, node->span->end_line, node->span->end_collumn);
    int next_level;
    if (node->type == AST_DECLARATION_LIST || node->type == AST_CODE_LIST || node->type == AST_PARAM_LIST ||
        node->type == AST_COMMAND_LIST || node->type == AST_EXPR_LIST) {
        next_level = level;
    } else {
        next_level = level + 1;
    }

    for (int i = 0; i < 4; i++) {
        astPrintDebug(node->children[i], next_level);
    }
    printIdent(level * 2, ' ');
    printf(")\n");
}
