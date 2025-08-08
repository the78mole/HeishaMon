#ifndef LITTLEFS_H
#define LITTLEFS_H

#ifdef NATIVE_SIM

#include "native_mocks.h"

// Use the File class from native_mocks.h
typedef MockLittleFS::MockFile File;

#endif // NATIVE_SIM
#endif // LITTLEFS_H