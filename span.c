//
// Created by vini84200 on 12/9/23.
//

#include "span.h"

#include "ast.h"
#include "hash.h"
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Span *emptySpan() {
    Span *span = (Span *) malloc(sizeof(Span));
    span->line = 0;
    span->collumn = 0;
    span->end_line = 0;
    span->end_collumn = 0;
    return span;
}
Span *newSpan(int line, int column, int end_line, int end_column) {
    Span *span = (Span *) malloc(sizeof(Span));
    span->line = line;
    span->collumn = column;
    span->end_line = end_line;
}
Span *newSpanInline(int line, int column, int length) {
}
Span *newSpanFromSpan(Span *span) {
    Span *new_span = (Span *) malloc(sizeof(Span));
    memcpy(new_span, span, sizeof(Span));
    return new_span;
}
void deleteSpan(Span *span) {
    free(span);
}
Span *add_spans(Span *span1, Span *span2) {
    Span *new_span = (Span *) malloc(sizeof(Span));
    int min_line = span1->line < span2->line ? span1->line : span2->line;
    new_span->line = min_line;
    if (span1->line == span2->line) {
        // If the lines are the same, use the smallest column
        new_span->collumn = span1->collumn < span2->collumn ? span1->collumn : span2->collumn;
    } else {
        // If the lines are different, use the column of the first
        new_span->collumn = span1->line < span2->line ? span1->collumn : span2->collumn;
    }
    int max_end_line = span1->end_line > span2->end_line ? span1->end_line : span2->end_line;
    new_span->end_line = max_end_line;
    if (span1->end_line == span2->end_line) {
        // If the lines are the same, use the biggest column
        new_span->end_collumn = span1->end_collumn > span2->end_collumn ? span1->end_collumn : span2->end_collumn;
    } else {
        // If the lines are different, use the column of the last
        new_span->end_collumn = span1->end_line > span2->end_line ? span1->end_collumn : span2->end_collumn;
    }

    return new_span;
}
struct YYLTYPE locationSpanning(struct YYLTYPE begin, struct YYLTYPE end) {
    struct YYLTYPE location;
    location.first_line = begin.first_line;
    location.first_column = begin.first_column;
    location.last_line = end.last_line;
    location.last_column = end.last_column;
    return location;
}
Span *spanFromLocation(struct YYLTYPE *location) {
    Span *span = (Span *) malloc(sizeof(Span));
    span->line = location->first_line;
    span->collumn = location->first_column;
    span->end_line = location->last_line;
    span->end_collumn = location->last_column;
    return span;
}
void printLocation(struct YYLTYPE location) {
    printf("Line %d:%d to %d:%d\n", location.first_line, location.first_column, location.last_line, location.last_column);
}
