//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//
//
// Created by vini84200 on 11/9/23.
//

#ifndef ETAPA2_ERRORHANDLER_H
#define ETAPA2_ERRORHANDLER_H

#define SYNTAX_ERROR 3
#define UNKNOW_ERROR 4

#define MAX_LINE_FOR_REPORT 512

void yyerror(const char *s);
void handleSyntaxError();

#endif//ETAPA2_ERRORHANDLER_H
