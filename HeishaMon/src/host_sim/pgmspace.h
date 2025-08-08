#pragma once

#ifdef HOST_SIM

#include "Arduino.h"

// PROGMEM macros for host simulation
#define PROGMEM
#define PSTR(s) (s)

#ifndef F
#define F(string_literal) (string_literal)
#endif

#define FPSTR(pstr_pointer) (pstr_pointer)

// pgmspace functions
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_float(addr) (*(const float *)(addr))
#define pgm_read_ptr(addr) (*(void * const *)(addr))

#define pgm_read_byte_near(addr) pgm_read_byte(addr)
#define pgm_read_word_near(addr) pgm_read_word(addr)
#define pgm_read_dword_near(addr) pgm_read_dword(addr)
#define pgm_read_float_near(addr) pgm_read_float(addr)
#define pgm_read_ptr_near(addr) pgm_read_ptr(addr)

#define pgm_read_byte_far(addr) pgm_read_byte(addr)
#define pgm_read_word_far(addr) pgm_read_word(addr)
#define pgm_read_dword_far(addr) pgm_read_dword(addr)
#define pgm_read_float_far(addr) pgm_read_float(addr)
#define pgm_read_ptr_far(addr) pgm_read_ptr(addr)

// String functions
#define strcpy_P(dest, src) strcpy((dest), (src))
#define strcat_P(dest, src) strcat((dest), (src))
#define strcmp_P(a, b) strcmp((a), (b))
#define strncmp_P(a, b, n) strncmp((a), (b), (n))
#define strlen_P(s) strlen(s)
#define strchr_P(s, c) strchr((s), (c))
#define strstr_P(a, b) strstr((a), (b))

#define sprintf_P sprintf
#define snprintf_P snprintf
#define vsnprintf_P vsnprintf

// Memory functions
#define memcpy_P(dest, src, num) memcpy((dest), (src), (num))
#define memcmp_P(a, b, n) memcmp((a), (b), (n))

typedef const char* PGM_P;
typedef const void* PGM_VOID_P;
typedef const unsigned char* PGM_UCHAR_P;

// Flash string helper for Arduino compatibility
class __FlashStringHelper;

#ifndef F
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
#endif

#endif // HOST_SIM