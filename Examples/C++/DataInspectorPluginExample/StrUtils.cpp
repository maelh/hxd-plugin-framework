#include "StrUtils.h"

#include <algorithm> 
#include <cwctype>


// string trimming functions from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring#217605

void ltrim(std::wstring &s)
{
	s.erase(
		s.begin(),
		std::find_if(s.begin(), s.end(),
			[](int ch) { return !std::iswspace(ch); }
		)
	);
}

void rtrim(std::wstring &s)
{
	s. erase(
		std::find_if(s.rbegin(), s.rend(),
			[](int ch) { return !std::iswspace(ch); }
		).base(),
		s.end()
	);
}

void trim(std::wstring &s)
{
	ltrim(s);
	rtrim(s);
}

std::wstring ltrim_copy(std::wstring s)
{
	ltrim(s);
	return s;
}

std::wstring rtrim_copy(std::wstring s)
{
	rtrim(s);
	return s;
}

std::wstring trim_copy(std::wstring s)
{
	trim(s);
	return s;
}
