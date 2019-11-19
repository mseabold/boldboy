#include "console_logger.h"
#include <stdarg.h>
#include <stdio.h>

void ConsoleLogger::logWrite(LogLevel level, const char *fmt, ...) {
    va_list args;

    if(level <= mLevel) {
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}
