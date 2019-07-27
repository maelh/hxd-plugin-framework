#pragma once

#include <string>

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
