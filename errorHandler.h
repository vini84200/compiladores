// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//
//
// Created by vini84200 on 11/9/23.
//

#ifndef ETAPA2_ERRORHANDLER_H
#define ETAPA2_ERRORHANDLER_H

#define WRONG_ARG_COUNT_ERR 1
#define CANNOT_OPEN_FILE_ERR 2
#define SYNTAX_ERROR_ERR 3
#define SEMANTIC_ERROR_ERR 4
#define UNKNOW_ERROR 255

#define MAX_LINE_FOR_REPORT 512
#include "semanticError.h"

void yyerror(const char *s);
void handle_syntax_error();
int handle_wrong_arg_count(void);
int handle_cannot_open_file(char *argv);
_Noreturn void handle_semantic_errors(SemanticErrorList *error_list);
int handleUnrecognizedToken(char token, int line);
int printLineErr(const int line_number);
int printLineErrWithPtr(const int line_number, const int ch, const char *ptr_text);
int printLineErrWithHighlight(const int line_number, const int ch, const int count, const char *ptr_text);

_Noreturn void criticalError(const char *message);

#endif//ETAPA2_ERRORHANDLER_H
