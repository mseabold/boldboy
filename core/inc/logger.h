#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdarg.h>
#include <stddef.h>

//TODO Make this controllable in CMake
#define BOLDBOG_LOG_ENABLE

#ifdef BOLDBOG_LOG_ENABLE

#define ELOG(...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_ERROR, __VA_ARGS__); } while(0)
#define WLOG(...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_WARNING, __VA_ARGS__); } while(0)
#define VLOG(...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_VERBOSE, __VA_ARGS__); } while(0)
#define DLOG(...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_DEBUG, __VA_ARGS__); } while(0)
#define FLOG(...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_FUNCTION, __VA_ARGS__); } while(0)

#else

#define ELOG(...) ((void)0)
#define WLOG(...) ((void)0)
#define VLOG(...) ((void)0)
#define DLOG(...) ((void)0)
#define FLOG(...) ((void)0)

#endif

enum LogLevel {
    LOG_NONE,
    LOG_ERROR,
    LOG_WARNING,
    LOG_VERBOSE,
    LOG_DEBUG,
    LOG_FUNCTION
};

class Logger {
    public:
        Logger() { mLevel = LOG_NONE; }
        ~Logger() {}

        virtual void logWrite(LogLevel level, const char *fmt, ...) = 0;
        void setLevel(LogLevel level) { mLevel = level; }
        static void setLogger(Logger *logger) {sLogger = logger;}
        static Logger *sLogger;

    protected:
        LogLevel mLevel;

};

#endif /* __LOGGER_H__ */
