#include "StrUtils.h"

#include <algorithm> 
#include <cctype>
#include <locale>

// Based on https://stackoverflow.com/questions/14176123/correct-usage-of-strtol#14176593
// see also: https://stackoverflow.com/questions/194465/how-to-parse-a-string-to-an-int-in-c#6154614
TStrToBytesError str2int(const wchar_t* str, long* res, int base)
{
	// reject leading whitespace
	if (str == NULL || iswspace(str[0]))
		return stbeInvalidString;

	TStrToBytesError converr = stbeNone;

	// Reset errno to 0, for reliable evaluation later on. (Because, with a few
	// specific exceptions in stdlib, where documented, errno is only updated
	// when an error occurs.)
	int saved_errno = errno;
	errno = 0;

	wchar_t* endptr;
	*res = wcstol(str, &endptr, base);

	if (str == endptr || // nothing was converted (empty string or invalid 
						 // integer characters)
		*endptr != '\0') // unallowed characters after the last valid digit
	{
		converr = stbeInvalidString;
	}
	else if (errno == ERANGE)
	{
		if (*res == LONG_MIN)
			converr = stbeUnderflow;
		else if (*res == LONG_MAX)
			converr = stbeOverflow;
	}

	errno = saved_errno; // be transparent to other stdlib functions
	return converr;
}


// string trimming functions from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring#217605

void ltrim(std::wstring &s)
{
	s.erase(
		s.begin(),
		std::find_if(s.begin(), s.end(),
			[](int ch) { return !std::isspace(ch); }
		)
	);
}

void rtrim(std::wstring &s)
{
	s. erase(
		std::find_if(s.rbegin(), s.rend(),
			[](int ch) { return !std::isspace(ch); }
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
