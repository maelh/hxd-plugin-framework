#pragma once

#include <string>
#include "DataInspectorShared.h"

// Fails on an empty string, on leading or trailing white space, and also on any
// other leading or trailing characters, that are not part of the integer.
// Accepts leading + or -. Accepts a base prefix: 0 for octal and 0x or 0X for
// hexadecimal, if base = 0. base ranges from 0 to 36.
// Otherwise, assumes an integer format as specified by wcstol().
TStrToBytesError str2int(const wchar_t* str, long* res, int base = 0);

// trim from start (in place)
void ltrim(std::wstring &s);

// trim from end (in place)
void rtrim(std::wstring &s);

// trim from both ends (in place)
void trim(std::wstring &s);

// trim from start (copying)
std::wstring ltrim_copy(std::wstring s);

// trim from end (copying)
std::wstring rtrim_copy(std::wstring s);

// trim from both ends (copying)
std::wstring trim_copy(std::wstring s);
