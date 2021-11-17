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
    FCategory = tcSignedInteger;
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
    TFormattingOptions FormattingOptions, int& ConvertedByteCount,
    std::wstring& ConvertedStr)
{
    if (ByteCount >= sizeof(int32_t))
    {
        ConvertedByteCount = sizeof(int32_t);

        std::wostringstream ss;

        switch (FormattingOptions.IntegerBase)
        {
        case ibDecimal:
            ss << std::dec;
            break;
        case ibHexadecimal:
            ss << std::hex;

            switch (FormattingOptions.HexCasing)
            {
            case lcUpperCase:
                ss << std::uppercase;
                break;
            case lcLowerCase:
                ss << std::nouppercase;
                break;
            }
            break;
        }

        // WARNING: this approach will not work for (u)int8_t, which will be
        // interpreted as a character, not an 8 bit integer!
        // As alternative, you can use swprintf() as exemplified in
        // DataInspectorPluginExample.c
        ss << *((int32_t*)Bytes);
        ConvertedStr = ss.str();
        
        if (FormattingOptions.IntegerBase == ibHexadecimal)
            switch (FormattingOptions.HexBaseIndication)
            {
            case hbiPascalAndMotorola:
                ConvertedStr = L"$" + ConvertedStr;
                break;
            case hbiC:
                ConvertedStr = L"0x" + ConvertedStr;
                break;
            case hbiIntelNoLeadingZero:
            case hbiIntelLeadingZero:
                if (FormattingOptions.HexBaseIndication == hbiIntelLeadingZero &&
                    (ConvertedStr[0] < L'0' || ConvertedStr[0] > L'9'))
                    ConvertedStr = L'0' + ConvertedStr;

                if (FormattingOptions.HexCasing == lcUpperCase)
                    ConvertedStr += L"h";
                else
                    ConvertedStr += L"H";
                break;
            }

        return btseNone;
    }
    else
    {
        ConvertedByteCount = 0;
        ConvertedStr = L""; // Warning: do not set to NULL, this will throw exceptions

        return btseBytesTooShort;
    }
}

TStrToBytesError TInt32Converter::StrToBytes(std::wstring Str,
    TFormattingOptions FormattingOptions, std::vector<uint8_t>& ConvertedBytes)
{
    static_assert(sizeof(long) >= sizeof(int32_t),
        "str2int must return an integer of at least sizeof(int32_t)");

    int base;
    switch (FormattingOptions.IntegerBase) 
    {
    case ibDecimal:
        base = 10;
        break;
    case ibHexadecimal:
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

TBytesToIntError TInt32Converter::AsInt64(uint8_t* Bytes, int ByteCount,
    int& ConvertedByteCount, int64_t& ConvertedInt)
{
    if (ByteCount >= sizeof(int32_t))
    {
        ConvertedByteCount = sizeof(int32_t);
        ConvertedInt = *(int32_t*)(Bytes);
        return btieNone;
    }
    else
    {
        ConvertedByteCount = 0;
        ConvertedInt = 0;

        return btieBytesTooShort;
    }
}

// not supported, just a dummy function to satisfy class requirements
TBytesToIntError TInt32Converter::AsUInt64(uint8_t* Bytes, int ByteCount,
    int& ConvertedByteCount, uint64_t& ConvertedInt)
{
    return btieNone;
}
