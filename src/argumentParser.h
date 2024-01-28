#ifndef COMPILADORES_ARGUMENTPARSER_H
#define COMPILADORES_ARGUMENTPARSER_H

#include <stdbool.h>
#include <stdio.h>

typedef enum OutputType_t {
    OUTPUT_TYPE_CHECK,
    OUTPUT_TYPE_INTERMEDIATE_CODE,
    OUTPUT_TYPE_ASSEMBLY,
} OutputType;

typedef struct Args_t {
    bool debug;
    bool help;
    FILE *inputFile;
    FILE *outputFile;
    OutputType outputType;

} Args;

Args parseArgs(int argc, char **argv);

#endif//COMPILADORES_ARGUMENTPARSER_H
