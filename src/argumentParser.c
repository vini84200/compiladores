#include "argumentParser.h"
#include "errorHandler.h"
#include <string.h>

void parseFlag(char *flag, Args *args) {
    if (strcmp(flag, "-d") == 0 || strcmp(flag, "--debug") == 0) {
        args->debug = true;
    }
    else if (strcmp(flag, "-h") == 0 || strcmp(flag, "--help") == 0) {
        args->help = true;
    }
    else {
        ERROR("Invalid flag %s", flag);
    }
}

Args parseArgs(int argc, char **argv) {
    Args args = {
        .debug = false,
        .help = false,
        .inputFile = NULL,
        .outputFile = stderr,
    };
    int pos = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            INFO("Flag %s", argv[i]);
            parseFlag(argv[i], &args);
        }
        else {
            pos++;
            if (pos == 1) {
                INFO("Opening input file %s", argv[i]);
                args.inputFile = fopen(argv[i], "r");
                if (args.inputFile == NULL) {
                    handle_cannot_open_file(argv[i]);
                }
            }
            else if (pos == 2) {
                INFO("Opening output file %s", argv[i]);
                args.outputFile = fopen(argv[i], "w");
                if (args.outputFile == NULL) {
                    handle_cannot_open_file(argv[i]);
                }
            } else {
                // Too many arguments
                ERROR("Too many arguments");
            }
        }
    }
    if (pos == 0 && !args.help) {
        // No input file
        handle_wrong_arg_count();
    }
    return args;
}

