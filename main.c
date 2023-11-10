//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "lib.h"
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>

// lex.yy.h
int yylex();
extern char *yytext;
extern FILE *yyin;
int yyparse();


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("call: ./etapa2 input.txt \n");
        exit(1);
    }
    if (0 == (yyin = fopen(argv[1], "r"))) {
        printf("Cannot open file %s... \n", argv[1]);
        exit(2);
    }

    printf("Iniciando parser...\n");
    initSymbolTable();
    yyparse();
    printf("Leitura concluída com sucesso!\n");
    hashPrint(getSymbolTable());

}
