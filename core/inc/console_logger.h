#ifndef __CONSOLE_LOGGER_H__
#define __CONSOLE_LOGGER_H__

#include "logger.h"

class ConsoleLogger : public Logger {
    void logWrite(const char *fmt, va_list args);
};

#endif /* __CONSOLE_LOGGER_H__ */
