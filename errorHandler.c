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

int handle_wrong_arg_count(void) {
    fprintf(stderr, "call: ./etapa2 input.txt \n");
    exit(WRONG_ARG_COUNT_ERR);
}
int handle_cannot_open_file(char *argv) {
    fprintf(stderr, "[ERROR] Cannot open file %s... \n", argv);
    exit(CANNOT_OPEN_FILE_ERR);
}
int handleUnrecognizedToken(char token, int line) {
    fprintf(stderr, "[ERROR] Token '%c' not recognized at %d:%d\n", token, line, getCollumn());
    printLineErrWithPtr(line, getCollumn(), "Unrecognized token");
    exit(SYNTAX_ERROR_ERR);
}

void handle_unknown_error(const char *s) {
    fprintf(stderr, "[ERROR] Line %d: %s\n", getLineNumber(), s);
    fprintf(stderr, "Unknow error\n");
    exit(UNKNOW_ERROR);
}
void yyerror(const char *s) {
    if (strcmp(s, "syntax error") == 0) {
        handle_syntax_error();
    } else {
        handle_unknown_error(s);
    }
}

char *getLine(char *line, const int line_number) {
    extern FILE *yyin;
    FILE *file = yyin;
    int lineCount = 1;
    rewind(file);
    while (fgets(line, MAX_LINE_FOR_REPORT, file) != NULL) {
        if (lineCount == line_number) {
            // Remove the \n from the end of the line
            line[strcspn(line, "\n")] = 0;
            return line;
        }
        lineCount++;
    }
    return NULL;
}

int printLineErr(const int line_number) {
    char line[MAX_LINE_FOR_REPORT];
    char *line_read = getLine(line, line_number);
    if (line_read != NULL) {
        fprintf(stderr, "\t  ... | \n");
        fprintf(stderr, "\t%5d | %s\n", line_number, line_read);
        fprintf(stderr, "\t  ... | \n");
    } else {
        fprintf(stderr, "Line not found\n");
    }
    fprintf(stderr, "Compilation failed\n");
    return 0;
}

int printLineErrWithPtr(const int line_number, const int ch, const char *ptr_text) {
    // Print the line where the error occurred with a pointer to the char
    char line[MAX_LINE_FOR_REPORT];
    char prev_line[MAX_LINE_FOR_REPORT];
    char next_line[MAX_LINE_FOR_REPORT];
    // TODO: get the line before and after the error without rescaning the file
    char *line_read = getLine(line, line_number);
    char *prev_line_read = getLine(prev_line, line_number - 1);
    char *next_line_read = getLine(next_line, line_number + 1);
    if (line_read != NULL) {
        fprintf(stderr, "\t%5d | %s\n", line_number - 1, prev_line_read);
        fprintf(stderr, "\t%5d | %s\n", line_number, line_read);
        fprintf(stderr, "\t      | ");
        for (int i = 1; i < ch; i++) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^ %s\n", ptr_text);
        fprintf(stderr, "\t%5d | %s\n", line_number + 1, next_line_read);
        return 0;
    } else {
        fprintf(stderr, "Line not found\n");
        return 1;
    }
}
int printLineErrWithHighlight(const int line_number, const int ch, const int count, const char *ptr_text) {
    if (count <= 1) {
        return printLineErrWithPtr(line_number, ch, ptr_text);
    }
    char line[MAX_LINE_FOR_REPORT];
    char prev_line[MAX_LINE_FOR_REPORT];
    char next_line[MAX_LINE_FOR_REPORT];
    // TODO: get the line before and after the error without rescaning the file
    char *line_read = getLine(line, line_number);
    char *prev_line_read = getLine(prev_line, line_number - 1);
    char *next_line_read = getLine(next_line, line_number + 1);
    if (line_read != NULL) {
        fprintf(stderr, "\t%5d | %s\n", line_number - 1, prev_line_read);
        fprintf(stderr, "\t%5d | %s\n", line_number, line_read);
        fprintf(stderr, "\t      | ");
        for (int i = 1; i < ch; i++) {
            fprintf(stderr, " ");
        }
        for (int i = 0; i < count; i++) {
            fprintf(stderr, "~");
        }
        fprintf(stderr, " %s\n", ptr_text);
        fprintf(stderr, "\t%5d | %s\n", line_number + 1, next_line_read);
        return 0;
    } else {
        fprintf(stderr, "Line not found\n");
        return 1;
    }
}

void handle_syntax_error() {
    // Print the line where the error occurred
    fprintf(stderr, "[ERROR] Syntax error at line %d\n", getLineNumber());

    // Read the line where the error occurred
    // printLineErr(getLineNumber());
    printLineErrWithHighlight(getLineNumber(), getCollumn(), yyleng, "Syntax error");
    exit(SYNTAX_ERROR_ERR);
}
