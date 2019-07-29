#pragma once

#ifdef __cplusplus  
extern "C" {
#endif 

#include "DataInspectorShared.h"

// Allocates a new wide string with malloc(), copies str into it, and returns
// the copy. The returned string has to be explicitly deallocated with free().
wchar_t* wcsclone(const wchar_t* str);

// Modifies str's content to trim from the right, and returns the incremented
// pointer to trim from the left.
wchar_t* trim(wchar_t* str);

// Fails on an empty string, on leading or trailing white space, and also on any
// other leading or trailing characters, that are not part of the integer.
// Accepts leading + or -. Accepts a base prefix: 0 for octal and 0x or 0X for
// hexadecimal, if base = 0. base ranges from 0 to 36.
// Otherwise, assumes an integer format as specified by wcstol().
TStrToBytesError str2int(const wchar_t* str, long* res, int base);

TStrToBytesError str2uint(const wchar_t* str, unsigned long* res, int base);

#ifdef __cplusplus  
}
#endif 