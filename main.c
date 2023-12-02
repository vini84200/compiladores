//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "errorHandler.h"
#include "ast.h"
#include "lex.yy.h"
#include "lib.h"
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>

// lex.yy.h
int yyparse();


int main(const int argc, char **argv) {
    if (argc < 3) {
        return handle_wrong_arg_count();
    }
    if (0 == (yyin = fopen(argv[1], "r"))) {
        return handle_cannot_open_file(argv[1]);
    }
    open_output_file(argv[2]);

    initSymbolTable();
    printf("Iniciando parser...\n");
    yyparse();
    printf("Leitura concluída com sucesso!\n");
    hashPrint(getSymbolTable());

    return 0;
}
