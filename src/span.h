//
// Created by vini84200 on 12/9/23.
//

#ifndef SPAN_H
#define SPAN_H

typedef struct Span_t {
    int line;
    int collumn;
    int end_line;
    int end_collumn;
} Span;

struct YYLTYPE;

Span *emptySpan();
Span *spanFromLocation(struct YYLTYPE *location);
Span *newSpan(int line, int column, int end_line, int end_column);
Span *newSpanInline(int line, int column, int length);
Span *newSpanFromSpan(Span *span);
void deleteSpan(Span *span);
Span *add_spans(Span *span1, Span *span2);

struct YYLTYPE locationSpanning(struct YYLTYPE begin, struct YYLTYPE end);

void printLocation(struct YYLTYPE location);


#endif//SPAN_H
