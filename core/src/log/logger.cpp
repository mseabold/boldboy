#include "logger.h"

Logger *Logger::sLogger = NULL;

void Logger::log(LogLevel level, uint32_t zone, const char *fmt, ...) {
    va_list args;
    if(level <= mLevel && (zone & mZone) != 0) {
        va_start(args, fmt);
        logWrite(fmt, args);
        va_end(args);
    }
}

void Logger::setZone(uint32_t zone) {
    mZone = zone;
}
