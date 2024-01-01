
#include "logs.h"
LogLevel minLevel = L_DEBUG;
FILE *logsFile = NULL;

void setLogLevel(LogLevel level) {
    minLevel = level;
}
void initLogs() {
    setLogsFile(stderr);
    setLogLevel(L_DEBUG);
}
void setLogsFile(FILE *file) {
    logsFile = file;
}
