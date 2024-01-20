#ifndef COMPILADORES_ARGUMENTPARSER_H
#define COMPILADORES_ARGUMENTPARSER_H

#include <stdbool.h>
#include <stdio.h>

typedef struct Args_t {
    bool debug;
    bool help;
    FILE *inputFile;
    FILE *outputFile;
} Args;

Args parseArgs(int argc, char **argv);

#endif//COMPILADORES_ARGUMENTPARSER_H
