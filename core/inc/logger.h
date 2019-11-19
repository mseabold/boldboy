#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdarg.h>
#include <stddef.h>

//TODO Make this controllable in CMake
#define BOLDBOG_LOG_ENABLE

#ifdef BOLDBOG_LOG_ENABLE

#define ELOG(_fmt, ...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_ERROR, _fmt, __VA_ARGS__); } while(0)
#define WLOG(_fmt, ...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_WARNING, _fmt, __VA_ARGS__); } while(0)
#define VLOG(_fmt, ...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_VERBOSE, _fmt, __VA_ARGS__); } while(0)
#define DLOG(_fmt, ...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_DEBUG, _fmt, __VA_ARGS__); } while(0)
#define FLOG(_fmt, ...) do { if(Logger::sLogger) Logger::sLogger->logWrite(LOG_FUNCTION, _fmt, __VA_ARGS__); } while(0)

#else

#define ELOG(_fmt, ...) ((void)0)
#define WLOG(_fmt, ...) ((void)0)
#define VLOG(_fmt, ...) ((void)0)
#define DLOG(_fmt, ...) ((void)0)
#define FLOG(_fmt, ...) ((void)0)

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
