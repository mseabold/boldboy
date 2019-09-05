#include "serial_console.h"
#include <stdio.h>

SerialConsole::SerialConsole() {}
SerialConsole::~SerialConsole() {}

void SerialConsole::doXfer() {
    printf("%c", mSB);
    mSB = 0; //XXX Output only console for now
}
