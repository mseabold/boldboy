#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

//TODO Make this controllable in CMake
#define BOLDBOG_LOG_ENABLE

#ifdef BOLDBOG_LOG_ENABLE

#define ZONE_CORE 0x00000001
#define ZONE_CPU  0x00000002
#define ZONE_MMU  0x00000004
#define ZONE_PPU  0x00000008
#define ZONE_CART 0x00000010
#define ZONE_IO   0x00000020
#define ZONE_DMA  0x00000040

#define ELOG(_zone, ...) do { if(Logger::sLogger) Logger::sLogger->log(LOG_ERROR, _zone, __VA_ARGS__); } while(0)
#define WLOG(_zone, ...) do { if(Logger::sLogger) Logger::sLogger->log(LOG_WARNING, _zone, __VA_ARGS__); } while(0)
#define VLOG(_zone, ...) do { if(Logger::sLogger) Logger::sLogger->log(LOG_VERBOSE, _zone, __VA_ARGS__); } while(0)
#define DLOG(_zone, ...) do { if(Logger::sLogger) Logger::sLogger->log(LOG_DEBUG, _zone, __VA_ARGS__); } while(0)
#define FLOG(_zone, ...) do { if(Logger::sLogger) Logger::sLogger->log(LOG_FUNCTION, _zone, __VA_ARGS__); } while(0)

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
        Logger() { mLevel = LOG_NONE; mZone = 0;}
        ~Logger() {}

        void log(LogLevel level, uint32_t zone, const char *fmt, ...);
        virtual void logWrite(const char *fmt, va_list args) = 0;
        void setLevel(LogLevel level) { mLevel = level; }
        void setZone(uint32_t zone);
        static void setLogger(Logger *logger) {sLogger = logger;}
        static Logger *sLogger;

    protected:
        LogLevel mLevel;
        uint32_t mZone;

};

#endif /* __LOGGER_H__ */
