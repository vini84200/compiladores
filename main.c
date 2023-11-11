//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "errorHandler.h"
#include "lib.h"
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>

// lex.yy.h
extern FILE *yyin;
int yyparse();


int main(int argc, char **argv) {
    if (argc < 2) {
        return handle_wrong_arg_count();
    }
    if (0 == (yyin = fopen(argv[1], "r"))) {
        return handle_cannot_open_file(argv[1]);
    }

    printf("Iniciando parser...\n");
    initSymbolTable();
    yyparse();
    printf("Leitura concluída com sucesso!\n");
    hashPrint(getSymbolTable());
    return 0;
}
