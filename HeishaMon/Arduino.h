#ifndef ARDUINO_H
#define ARDUINO_H

#ifdef NATIVE_SIM
#include "native_mocks.h"
#else
#error "This Arduino.h mock is only for NATIVE_SIM builds"
#endif

#endif // ARDUINO_H