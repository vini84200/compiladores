#ifndef YYLTYPE_HEADER
#define YYLTYPE_HEADER

typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    unsigned source;
} YYLTYPE;

#endif// !YYLTYPE_HEADER
