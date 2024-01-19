//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "main.h"
#include "../generated/y.tab.h"
#include "ast.h"
#include "errorHandler.h"
#include "generateCode.h"
#include "lib.h"
#include "logs.h"
#include "semantics.h"
#include "tac.h"
#include <stdio.h>
#include <stdlib.h>

// lex.yy.h
extern FILE *yyin;
int yyparse();


int main(const int argc, char **argv) {
    initLogs();
    if (argc < 2) {
        return handle_wrong_arg_count();
    }
    if (0 == (yyin = fopen(argv[1], "r"))) {
        return handle_cannot_open_file(argv[1]);
    }

    initSymbolTable();
    INFO("Iniciando parser...");
    yyparse();
    INFO("Leitura concluída com sucesso!");

    INFO("Iniciando análise semântica...");
    SemanticAnalyzerResult *result = analyzeSemantics(getAST());
    if (result->error_list->first != NULL) {
        handle_semantic_errors(result->error_list);
    }
    destroySemanticAnalyzerResult(result);


    INFO("Análise semântica concluída com sucesso!");

    INFO("Iniciando geração de codigo intermediario")
    TacList *code = generateCode(getAST());
    // printTACList(code);
    INFO("Código gerado com sucesso")

    DEBUG("Código gerado:\n");
    printCode(code);


    destroyAST(getAST());
    setAST(NULL);

    return 0;
}
