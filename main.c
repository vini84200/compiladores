//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "ast.h"
#include "errorHandler.h"
#include "lex.yy.h"
#include "lib.h"
#include "semantics.h"
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>

// lex.yy.h
int yyparse();


int main(const int argc, char **argv) {
    if (argc < 2) {
        return handle_wrong_arg_count();
    }
    if (0 == (yyin = fopen(argv[1], "r"))) {
        return handle_cannot_open_file(argv[1]);
    }
    // open_output_file(argv[2]);

    initSymbolTable();
    printf("Iniciando parser...\n");
    yyparse();
    printf("Leitura concluída com sucesso!\n");
    // hashPrint(getSymbolTable());
    printf("Iniciando análise semântica...\n");
    SemanticAnalyzerResult *result = anylzeSemantic(getAST());
    if (result->error_list->first != NULL) {
        handle_semantic_errors(result->error_list);
    }
    destroySemanticAnalyzerResult(result);


    printf("Análise semântica concluída com sucesso!\n");

     destroyAST(getAST());
    setAST(NULL);

    return 0;
}
