//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//
//
// Created by vini84200 on 11/9/23.
//

#include "errorHandler.h"
#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void yyerror(const char *s) {
    if (strcmp(s, "syntax error") == 0) {
        handleSyntaxError();
    } else {
        fprintf(stderr, "Line %d: %s\n", getLineNumber(), s);
        fprintf(stderr, "Unknow error\n");
        exit(UNKNOW_ERROR);
    }
}

char *getLine(char *line, int lineNumber) {
    extern FILE *yyin;
    FILE *file = yyin;
    int lineCount = 1;
    rewind(file);
    while (fgets(line, MAX_LINE_FOR_REPORT, file) != NULL) {
        if (lineCount == lineNumber) {
            return line;
        }
        lineCount++;
    }
    return NULL;
}

int printLineErr(int lineNumber) {
    char line[MAX_LINE_FOR_REPORT];
    char *lineRead = getLine(line, lineNumber);
    if (lineRead != NULL) {
        fprintf(stderr, "\t ...| \n");
        fprintf(stderr, "\t %d | %s\n", lineNumber, lineRead);
        fprintf(stderr, "\t ...| \n");
    } else {
        fprintf(stderr, "Line not found\n");
    }
    fprintf(stderr, "Compilation failed\n");
    return 0;
}

void handleSyntaxError() {
    // Print the line where the error occurred
    fprintf(stderr, "Syntax error at line %d\n", getLineNumber());

    // Read the line where the error occurred
    printLineErr(getLineNumber());
    exit(SYNTAX_ERROR);
}
