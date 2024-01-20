//
// +--------------------------------------------------------------------------+
// |  Compiladores - 2023/2                                                   |
// |  VINICIUS JOSÉ FRITZEN -- 335519                                         |
// +--------------------------------------------------------------------------+
//

#include "main.h"
#include "../generated/y.tab.h"
#include "argumentParser.h"
#include "ast.h"
#include "errorHandler.h"
#include "generateCode.h"
#include "lib.h"
#include "logs.h"
#include "semantics.h"
#include "tac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// lex.yy.h
extern FILE *yyin;
int yyparse();

void printHelp();
int main(const int argc, char **argv) {
    initLogs();
    Args args = parseArgs(argc, argv);
    yyin = args.inputFile;
    FILE *intermediateCodeFile = args.outputFile;
    if (args.help) {
        printHelp();
        return 0;
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
    printCode(code, intermediateCodeFile);


    destroyAST(getAST());
    setAST(NULL);

    return 0;
}
void printHelp() {
    printf("Usage: ./compiladores [options] input_file [output_file]\n"
           "Options:\n"
           "  -h, --help\t\t\tShow this help message\n"
           "  -d, --debug\t\t\tEnable debug mode\n");
}
