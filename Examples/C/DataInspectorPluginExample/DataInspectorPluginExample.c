#include "DataInspectorPluginExample.h"

#include <stdio.h>      
#include <stdint.h>     
#include <inttypes.h>
#include <assert.h>

#include "utils.h"
#include "CStrUtils.h"

// This is the most basic implementation in C, which defines a new set of
// functions (refer to TDataTypeConverterPluginInterface) for each data type
// converter, and does not try to leverage polymorphy (which can be done in C
// as well, even if less comfortably) to minimize code.
//
// The main purpose of the C example is to ease translation of the plugin
// interface to other languages, with the tradeoff of increasing the effort for
// each individual plugin. For simple plugins, the additional effort should
// remain minimal.
//
// NOTE: Even for non OO-plugins the contract for CreateConverter expects that
// the plugin manages some state for each data converter, individually, and does
// not use the same global state for all of them. In other words,
// CreateConverter needs to return a pointer (or some other unique id) that can
// be used to access that state in calls to the other functions defined in
// TDataTypeConverterPluginInterface. The pointer returned by CreateConverter
// will be passed as ThisPtr parameter (also named Converter or Source) in the
// other functions.
// 
// Currently, the two cases requiring individual state are for returned strings
// and returned bytes, whose references should remain valid until the next
// function call:
// - BytesToStr returns a pointer to a string, that should remain valid, until
// the next call to BytesToStr for the same converter, or until DeleteConverter
// is called.
// - StrToBytes returns a pointer to an array of bytes, that should remain
// valid, until the next call to StrToBytes for the same converter, or until
// DeleteConverter is called.


typedef struct TExampleDTCInstance {
    // NOTE: the array sizes were chosen statically for the int32_t type, but
    // you may need to dynamically allocate memory for other types to not waste
    // memory; in this case, make sure to allocate and initialize in
    // CreateConverter, and deallocate in DeleteConverter (which usually means
    // the memory management will be slightly more complex than in this
    // example).
    // What matters is that each converter instance has its own memory, that
    // holds converted data returned by BytesToStr and StrToBytes (see the
    // comment at file start).
    wchar_t ConvertedStr[128]; // smaller should be fine, but depends on formatting
    uint8_t ConvertedBytes[sizeof(int32_t)];
} TExampleDTCInstance;

void* __stdcall CreateConverter(TConverterType ConvType, const wchar_t** Name,
    TDataTypeWidth* Width, int* MaxTypeSize, TByteOrders* SupportedByteOrders)
{
    // ConvType can be used to pass in class information to customize creation,
    // and to ease the implementation of constructor delegates.
    // See the Delphi and C++ examples to see how this is used to call the right
    // class constructors or factory functions in a generic manner.
    assert(ConvType == &ExampleDTCClass); 

    *Name = L"C - Int32";
    *Width = dtwFixed;
    *MaxTypeSize = sizeof(int32_t);
    *SupportedByteOrders = 1 << boLittleEndian | 1 << boBigEndian;

    TExampleDTCInstance* Converter = malloc(sizeof(TExampleDTCInstance));
    memset(Converter, 0, sizeof(TExampleDTCInstance));
    return Converter;
}

void __stdcall DeleteConverter(void* Converter)
{
    free(Converter);
}

void __stdcall AssignConverter(void* ThisPtr, void* Source)
{
    memcpy(((TExampleDTCInstance*)ThisPtr)->ConvertedBytes,
        ((TExampleDTCInstance*)Source)->ConvertedBytes,
        FIELD_SIZEOF(TExampleDTCInstance, ConvertedBytes));

    memcpy(((TExampleDTCInstance*)ThisPtr)->ConvertedStr,
        ((TExampleDTCInstance*)Source)->ConvertedStr,
        FIELD_SIZEOF(TExampleDTCInstance, ConvertedStr));
}

void __stdcall ChangeByteOrder(void* ThisPtr, uint8_t* Bytes, int ByteCount,
    TByteOrder TargetByteOrder)
{
    static_assert(sizeof(unsigned long) == sizeof(int32_t),
        "types must match in size to ensure a valid byte swapping");

    if (TargetByteOrder == boBigEndian && ByteCount >= sizeof(int32_t))
        *(int32_t*)Bytes = _byteswap_ulong(*(int32_t*)Bytes);
}

TBytesToStrError __stdcall BytesToStr(void* ThisPtr, uint8_t* Bytes,
    int ByteCount, TIntegerDisplayOption IntegerDisplayOption,
    int* ConvertedByteCount, const wchar_t** ConvertedStr)
{
    if (ByteCount >= sizeof(int32_t))
    {
        *ConvertedByteCount = sizeof(int32_t);

        wchar_t* FormatStr = NULL;
        switch (IntegerDisplayOption)
        {
        case idoDecimal:
            FormatStr = L"%" PRId32;
            break;
        case idoHexadecimalUpperCase:
            FormatStr = L"%" PRIX32;
            break;
        case idoHexadecimalLowerCase:
            FormatStr = L"%" PRIx32;
            break;
        }

        swprintf(((TExampleDTCInstance*)ThisPtr)->ConvertedStr,
            ARRAY_LENGTH(((TExampleDTCInstance*)ThisPtr)->ConvertedStr),
            FormatStr, *(int32_t*)(Bytes));

        *ConvertedStr = ((TExampleDTCInstance*)ThisPtr)->ConvertedStr;

        return btseNone;
    }
    else
    {
        *ConvertedByteCount = 0;
        *ConvertedStr = NULL;

        return btseBytesTooShort;
    }
}

TStrToBytesError __stdcall StrToBytes(void* ThisPtr, const wchar_t* Str,
    TIntegerDisplayOption IntegerDisplayOption, uint8_t** ConvertedBytes,
    int* ConvertedByteCount)
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
    wchar_t* cloned_str = wcsclone(Str);
    wchar_t* trimmed_str = trim(cloned_str);
    TStrToBytesError result = str2int(trimmed_str, &I, base);
    free(cloned_str);

    *ConvertedBytes = ((TExampleDTCInstance *)ThisPtr)->ConvertedBytes;
    (*(int32_t*)*ConvertedBytes) = (int32_t)I;
    
    *ConvertedByteCount = sizeof(int32_t);

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

TDataTypeConverterPluginInterface ExampleDTCClass =
{
    .ConverterType = &ExampleDTCClass,
    
    .CreateConverter = &CreateConverter,
    .DeleteConverter = &DeleteConverter,
    .AssignConverter = &AssignConverter,

    .ChangeByteOrder = &ChangeByteOrder,
    .BytesToStr = &BytesToStr,
    .StrToBytes = &StrToBytes
};
