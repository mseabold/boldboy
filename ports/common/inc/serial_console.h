#ifndef __SERIAL_CONSOLE_H__
#define __SERIAL_CONSOLE_H__

#include "io_serial.h"

class SerialConsole : public IoSerial {
    public:
        SerialConsole();
        ~SerialConsole();

        void doXfer();
};

#endif /* __SERIAL_CONSOLE_H__ */
