#include "CStrUtils.h"
#include "DataInspectorShared.h"

#include <stdlib.h>
#include <errno.h>
#include <wchar.h>
#include <wctype.h>
#include <limits.h>

wchar_t* wcsclone(const wchar_t* str)
{
    if (str == NULL)
        return NULL;

    size_t len = wcslen(str) + 1;
    wchar_t* result = malloc(len * sizeof(wchar_t));
    wmemcpy(result, str, len);

    return result;
}

wchar_t* trim(wchar_t* str)
{
    if (str == NULL)
        return NULL;

    size_t len = wcslen(str) + 1;
    wchar_t* str_start = str;
    wchar_t* str_end = str_start + len - 2;

    // trim right
    while (str_end != str_start && iswspace(*str_end))
        str_end--;
    str_end[1] = '\0';

    // trim left
    while (str_start != str_end && iswspace(*str_start))
        str_start++;

    return str_start;
}

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

TStrToBytesError str2uint(const wchar_t* str, unsigned long* res, int base)
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
    *res = wcstoul(str, &endptr, base);

    if (str == endptr || // nothing was converted (empty string or invalid 
                         // integer characters)
        *endptr != '\0') // unallowed characters after the last valid digit
    {
        converr = stbeInvalidString;
    }
    else if (errno == ERANGE)
    {
        if (*res == 0)
            converr = stbeUnderflow;
        else if (*res == ULONG_MAX)
            converr = stbeOverflow;
    }

    errno = saved_errno; // be transparent to other stdlib functions
    return converr;
}