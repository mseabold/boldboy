#include "console_logger.h"
#include <stdarg.h>
#include <stdio.h>

void ConsoleLogger::logWrite(const char *fmt, va_list args) {
    vprintf(fmt, args);
}
