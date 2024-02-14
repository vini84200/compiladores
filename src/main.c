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
#include "callsGraph.h"
#include "errorHandler.h"
#include "funcCode.h"
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

    AST *functionDeclarations = getAST()->children[1];
    ASTListIterator *it = createASTListIterator(functionDeclarations);
    CallsGraph *callsGraph = createCallsGraph();
    if (ASTIteratorDone(it)) {
        ERROR("Nenhuma função encontrada")
    } else {
        INFO("Encontradas funções")
    }
    while (!ASTIteratorDone(it)) {
        AST *function = getNextAST(it);
        INFO("Gerando código para função %s", function->symbol->value);
        FunctionCode *fc = generateFunctionCode(function->symbol, function->children[0]);
        INFO("Código gerado com sucesso para função %s", function->symbol->value);
        addFunction(callsGraph, function->symbol, fc->graphStart);
    }
    populateCallsGraph(callsGraph);
    int nFunctions = callsGraph->nNodes;
    INFO("Encontradas %d funções", nFunctions);

    CallsGraphIterator *callsGraphIterator = createCallsGraphIterator(callsGraph);
    while (hasNextCallsGraphNode(callsGraphIterator)) {
        CallsGraphNode *node = nextCallsGraphNode(callsGraphIterator);
        INFO("Analisando função %s", node->function->value);
        if (node->directVarUsage == NULL) {
            node->directVarUsage = getFunctionDirectVarUsage(node->startBlock, node->function);
        }
        node->completeVarUsage = createVarUsageList();
        for (int i = 0; i < node->directVarUsage->nUsages; i++) {
            addVarUsage(node->completeVarUsage, node->directVarUsage->usages[i]);
        }
        int nDependencies;
        HashEntry **dependencies = getDependencies(callsGraph, node->function, &nDependencies);
        for (int i = 0; i < nDependencies; i++) {
            CallsGraphNode *dependency = getFunctionNode(callsGraph, dependencies[i]);
            if (dependency->directVarUsage == NULL) {
                dependency->directVarUsage = getFunctionDirectVarUsage(dependency->startBlock, dependency->function);
            }
            VarUsageIterator *it = createVarUsageIterator(dependency->directVarUsage);
            it->filteroutLocal = true;
            it->filteroutParam = true;
            VarUsage *usage;
            while ((usage = nextVarUsage(it)) != NULL) {
                VarUsage *varUsage = getVarUsage(node->completeVarUsage, usage->var);
                if (varUsage == NULL) {
                    addVarUsage(node->completeVarUsage, usage);
                } else {
                    varUsage->isRead = varUsage->isRead || usage->isRead;
                    varUsage->isWritten = varUsage->isWritten || usage->isWritten;
                }
            }
        }
        INFO("Análise de variáveis concluída para função %s", node->function->value);
        printVarUsageList(node->completeVarUsage);
    }
    freeCallsGraphIterator(callsGraphIterator);


    printCallsGraph(callsGraph);

    destroyASTListIterator(it);


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
