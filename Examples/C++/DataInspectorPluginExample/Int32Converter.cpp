#include <stdlib.h>
#include <sstream>

#include "CStrUtils.h"
#include "StrUtils.h"

#include "Int32Converter.h"


TExternalDataTypeConverter* TInt32Converter::Create()
{
    return new TInt32Converter();
}

TInt32Converter::TInt32Converter()
{
    FTypeName = L"C++ - Int32";
    FFriendlyTypeName = FTypeName;
    FWidth = dtwFixed;
    FMaxTypeSize = sizeof(int32_t);
    FSupportedByteOrders = 1 << boLittleEndian | 1 << boBigEndian;
    FSupportsStrToBytes = TRUE;
}

void TInt32Converter::ChangeByteOrder(uint8_t* Bytes, int ByteCount,
    TByteOrder TargetByteOrder)
{
    static_assert(sizeof(unsigned long) == sizeof(int32_t),
        "types must match in size to ensure a valid byte swapping");

    if (TargetByteOrder == boBigEndian && ByteCount >= sizeof(int32_t))
       *(int32_t*)Bytes = _byteswap_ulong(*(int32_t*)Bytes);
}

TBytesToStrError TInt32Converter::BytesToStr(uint8_t* Bytes, int ByteCount,
    TIntegerDisplayOption IntegerDisplayOption, int& ConvertedByteCount,
    std::wstring& ConvertedStr)
{
    if (ByteCount >= sizeof(int32_t))
    {
        ConvertedByteCount = sizeof(int32_t);

        std::wostringstream ss;
        switch (IntegerDisplayOption)
        {
        case idoDecimal:
            ss << std::dec;
            break;
        case idoHexadecimalUpperCase:
            ss << std::hex << std::uppercase;
            break;
        case idoHexadecimalLowerCase:
            ss << std::hex << std::nouppercase;
            break;
        }

        // WARNING: this approach will not work for (u)int8_t, which will be
        // interpreted as a character, not an 8 bit integer!
        // As alternative, you can use swprintf() as exemplified in
        // DataInspectorPluginExample.c
        ss << *((int32_t*)Bytes);
        ConvertedStr = ss.str();

        return btseNone;
    }
    else
    {
        ConvertedByteCount = 0;
        ConvertedStr = L""; // Warning: do not return NULL, this will throw exceptions

        return btseBytesTooShort;
    }
}

TStrToBytesError TInt32Converter::StrToBytes(std::wstring Str,
    TIntegerDisplayOption IntegerDisplayOption,
    std::vector<uint8_t>& ConvertedBytes)
{
    static_assert(sizeof(long) >= sizeof(int32_t),
        "str2int must return an integer of at least sizeof(int32_t)");

    int base;
    switch (IntegerDisplayOption) 
    {
    case idoDecimal:
        base = 10;
        break;
    case idoHexadecimalUpperCase:
    case idoHexadecimalLowerCase:
        base = 16;
        break;
    }

    long I;
    trim(Str);
    TStrToBytesError result = str2int(Str.c_str(), &I, base);

    // hexadecimal numbers are always handled as unsigned integers
    if (result == stbeOverflow)
    {
        unsigned long UI;
        result = str2uint(Str.c_str(), &UI, base);
        I = (long)UI;
    }

    ConvertedBytes.resize(sizeof(int32_t));
    *((int32_t*)&ConvertedBytes[0]) = (int32_t)I;

    if (sizeof(long) > sizeof(int32_t))
    {
        if (result == stbeNone)
            if (I < INT32_MIN)
                return stbeUnderflow;
            else if (I > INT32_MAX)
                return stbeOverflow;
    }

    return result;
}
