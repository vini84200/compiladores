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
#include "../generated/lex.yy.h"
#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int handle_wrong_arg_count(void) {
    fprintf(stderr, "call: ./etapa2 input.txt output\n");
    exit(WRONG_ARG_COUNT_ERR);
}
int handle_cannot_open_file(char *argv) {
    fprintf(stderr, "[ERROR] Cannot open file %s... \n", argv);
    exit(CANNOT_OPEN_FILE_ERR);
}

void semantic_error_print_spans(SemanticError *error) {
    // Print the line where the error occurred
    // It could be just one span or two spans

    if (error->secondary_span == NULL) {
        // If the error has only one span
        printLineErrWithHighlight(error->span->line, error->span->collumn, error->span->end_collumn - error->span->collumn + 1, error->span_text);
    } else {
        // If the error has two spans
        // Determine which span is the first and which is the second
        if (error->span->line == error->secondary_span->line) {
            // Same line
            printLine(error->span->line - 1);
            printLine(error->span->line);
            printDoubleHighlight(error->span->collumn, error->span->end_collumn - error->span->collumn + 1, error->span_text, error->secondary_span->collumn, error->secondary_span->end_collumn - error->secondary_span->collumn + 1, error->secondary_span_text);
            printLine(error->span->line + 1);
        } else if (error->span->line < error->secondary_span->line) {
            // First span is before the second
            int diff = error->secondary_span->line - error->span->line;
            if (diff < 4) {
                // If the spans are close, print continuously

                printLine(error->span->line - 1);
                printLine(error->span->line);
                printHighlight(error->span->collumn, error->span->end_collumn - error->span->collumn + 1, error->span_text);
                for (int i = 0; i < diff; i++) {
                    printLine(error->span->line + i + 1);
                }
                printHighlight(error->secondary_span->collumn, error->secondary_span->end_collumn - error->secondary_span->collumn + 1, error->secondary_span_text);
                printLine(error->secondary_span->line + 1);

            } else {
                // If the spans are far, print the first span and then the second span
                printLineErrWithHighlight(error->span->line, error->span->collumn, error->span->end_collumn - error->span->collumn + 1, error->span_text);
                fprintf(stderr, "       ...\n");
                printLineErrWithHighlight(error->secondary_span->line, error->secondary_span->collumn, error->secondary_span->end_collumn - error->secondary_span->collumn + 1, error->secondary_span_text);
            }
        } else {
            // Second span is before the first
            int diff = error->span->line - error->secondary_span->line;
            if (diff < 4) {
                // If the spans are close, print continuously
                printLine(error->secondary_span->line - 1);
                printLine(error->secondary_span->line);
                printHighlight(error->secondary_span->collumn, error->secondary_span->end_collumn - error->secondary_span->collumn + 1, error->secondary_span_text);
                for (int i = 0; i < diff; i++) {
                    printLine(error->secondary_span->line + i + 1);
                }
                printHighlight(error->span->collumn, error->span->end_collumn - error->span->collumn + 1, error->span_text);
                printLine(error->span->line + 1);
            } else {
                // If the spans are far, print the first span and then the second span
                printLineErrWithHighlight(error->secondary_span->line, error->secondary_span->collumn, error->secondary_span->end_collumn - error->secondary_span->collumn + 1, error->secondary_span_text);
                fprintf(stderr, "       ...\n");
                printLineErrWithHighlight(error->span->line, error->span->collumn, error->span->end_collumn - error->span->collumn + 1, error->span_text);
            }
        }
    }
}

void handle_semantic_errors(SemanticErrorList *error_list) {
    SemanticErrorIterator *iterator = newSemanticErrorIterator(error_list);
    while (!semanticErrorIteratorDone(iterator)) {
        SemanticError *error = semanticErrorIteratorNext(iterator);
        fprintf(stderr, "[ERROR] %s at %d:%d\n", error->message, error->span->line, error->span->collumn);
        semantic_error_print_spans(error);
    }
    exit(SEMANTIC_ERROR_ERR);
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
void yyerror(/* struct YYLTYPE *pos, */ const char *s) {
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

int printLine(const int line_number) {
    char line[MAX_LINE_FOR_REPORT];
    char *line_read = getLine(line, line_number);
    if (line_read != NULL) {
        fprintf(stderr, "\t%5d | %s\n", line_number - 1, line);
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
void criticalError(const char *message) {
    fprintf(stderr, "[ERROR] %s\n", message);
    exit(UNKNOW_ERROR);
}

void handle_syntax_error() {
    // Print the line where the error occurred
    fprintf(stderr, "[ERROR] Syntax error at line %d\n", getLineNumber());

    // Read the line where the error occurred
    // printLineErr(getLineNumber());
    printLineErrWithHighlight(getLineNumber(), getCollumn(), yyleng, "Syntax error");
    exit(SYNTAX_ERROR_ERR);
}
int printHighlight(const int column, const int count, const char *ptr_text) {

    fprintf(stderr, "\t      | ");
    for (int i = 1; i < column; i++) {
        fprintf(stderr, " ");
    }
    for (int i = 0; i < count; i++) {
        fprintf(stderr, "~");
    }
    fprintf(stderr, " %s\n", ptr_text);
    return 0;
}
int printDoubleHighlight(const int column, const int count, const char *ptr_text, const int column2, const int count2, const char *ptr_text2) {
    /*
     * Example:
     * 1 | int main() {
     * 2 |     int a = 1;
     *   |     ~~~    ^^
     *   |     |      |
     *   |     |      +-- ptr_text2
     *   |     +--------- ptr_text
     */

    int min_column = column < column2 ? column : column2;
    int max_column = column > column2 ? column : column2;
    int first_count = column < column2 ? count : count2;
    int second_count = column > column2 ? count : count2;
    int between_lines = max_column - min_column;
    int spaces_before = min_column - 1;
    int spaces_between = between_lines - first_count;
    const char *first_ptr_text = column < column2 ? ptr_text : ptr_text2;
    const char *second_ptr_text = column > column2 ? ptr_text : ptr_text2;
    fprintf(stderr, "\t      | ");
    for (int i = 0; i < spaces_before; i++) {
        fprintf(stderr, " ");
    }
    for (int i = 0; i < first_count; i++) {
        fprintf(stderr, "~");
    }
    for (int i = 0; i < spaces_between; i++) {
        fprintf(stderr, " ");
    }

    for (int i = 0; i < second_count; i++) {
        fprintf(stderr, "~");
    }
    fprintf(stderr, " \n");
    fprintf(stderr, "\t      | ");
    for (int i = 0; i < spaces_before; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "|");
    for (int i = 0; i < first_count + spaces_between - 1; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "|\n");
    fprintf(stderr, "\t      | ");
    for (int i = 0; i < spaces_before; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "|");
    for (int i = 0; i < first_count + spaces_between - 1; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "+");
    for (int i = 0; i < second_count; i++) {
        fprintf(stderr, "-");
    }
    fprintf(stderr, " %s\n", second_ptr_text);
    fprintf(stderr, "\t      | ");
    for (int i = 0; i < spaces_before; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "+");
    for (int i = 0; i < first_count + spaces_between + second_count; i++) {
        fprintf(stderr, "-");
    }
    fprintf(stderr, " %s\n", first_ptr_text);


    return 0;
}
