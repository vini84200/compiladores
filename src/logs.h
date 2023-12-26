#ifndef LOGS_HEADER
#define LOGS_HEADER
#include <stdio.h>
#include <stdlib.h>

#define C_ESC "\033"
#define C_RED C_ESC "[0;31m"
#define C_RED_BOLD C_ESC "[1;31m"
#define C_YELLOW C_ESC "[0;33m"
#define C_RESET C_ESC "[0m"


typedef enum LogLevel_t {
    L_DEBUG = 1,
    L_INFO = 2,
    L_WARN = 3,
    L_ERROR = 4,
    L_CRITICA = 5,
    L_MAX_LEVELS = 6
} LogLevel;

#define GREY "\x1b[35;20m"
#define CYAN "\x1b[36;20m"
#define YELLOW "\x1b[33;20m"
#define RED "\x1b[31;20m"
#define WHITE_RED "\x1b[41;1m"
#define BOLD_RED "\x1b[31;1m"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static const char *logLevelString[L_MAX_LEVELS] = {
        [L_DEBUG] = GREY "DEBUG" C_RESET,
        [L_INFO] = CYAN "INFO" C_RESET,
        [L_WARN] = YELLOW "WARN" C_RESET,
        [L_ERROR] = RED "ERROR" C_RESET,
        [L_CRITICA] = WHITE_RED "CRITICAL" C_RESET};
#pragma GCC diagnostic pop


extern LogLevel minLevel;

#define LOG_(LEVEL, FORMAT, args...)                                                                     \
    if (LEVEL >= minLevel) {                                                                             \
        fprintf(stderr, "%8s(%2u) %s: " FORMAT "\n", __FILE__, __LINE__, logLevelString[LEVEL], ##args); \
    }

#define DEBUG(FORMAT, args...) LOG_(L_DEBUG, FORMAT, ##args)
#define INFO(FORMAT, args...) LOG_(L_INFO, FORMAT, ##args)
#define WARN(FORMAT, args...) LOG_(L_WARN, FORMAT, ##args)
#define ERROR(FORMAT, args...) LOG_(L_ERROR, FORMAT, ##args)
#define CRITICAL(FORMAT, args...) LOG_(L_CRITICA, FORMAT, ##args)

#endif// !LOGS_HEADER
