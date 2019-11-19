#ifndef __CONSOLE_LOGGER_H__
#define __CONSOLE_LOGGER_H__

#include "logger.h"

class ConsoleLogger : public Logger {
    void logWrite(LogLevel level, const char *fmt, ...);
};

#endif /* __CONSOLE_LOGGER_H__ */
