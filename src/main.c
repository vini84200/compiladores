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
#include "generateAssembly.h"
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

    if (args.outputType == OUTPUT_TYPE_CHECK) {
        return 0;
    }

    INFO("Iniciando geração de codigo intermediario")
    TacList *code = generateCode(getAST());
    // printTACList(code);
    INFO("Código gerado com sucesso")

    if (args.outputType == OUTPUT_TYPE_INTERMEDIATE_CODE) {
        printCode(code, intermediateCodeFile);
        return 0;
    }

    if (args.outputType == OUTPUT_TYPE_ASSEMBLY) {
        INFO("Iniciando geração de código assembly")
        generateAssembly(code, getAST(), intermediateCodeFile);
        INFO("Código assembly gerado com sucesso")
    }
    INFO("Encerrando programa...")

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
