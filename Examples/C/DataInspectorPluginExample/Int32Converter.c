#include <stdio.h>      
#include <stdint.h>     
#include <inttypes.h>
#include <assert.h>

#include "utils.h"
#include "CStrUtils.h"

#include "Int32Converter.h"

// This is the most basic implementation in C, which defines all the functions
// declared in DataInspectorPluginInterface.h in a minimally possible way.
//
// If more than one datatype converter is to be provided by a plugin/DLL, you
// have to use the parameters ClassIDOrFactoryFunc or ThisPtr to differentiate
// between them. The C++ plugin does this automatically, by delegating to the
// appropriate class/method, but the ideal approach will vary by language. As
// the main point of the C plugin example is to keep things as simple and
// neutral as possible, it only implements one datatype converter.
//
// HxD will create several instances of a datatype converter, as needed.
// StrToBytes() will return an array of bytes, and BytesToStr() will return a
// string. The returned values have to stay valid (remain in memory) until the
// next call to StrToBytes() or BytesToStr(), respectively.
// That means, you need to store this temporary data until the next call of said
// functions, and can modify the data only once you enter the function again.
// You should store this data for each instance separately, as done in this
// example plugin.
//
// The instances returned by CreateConverter() will be passed by HxD to the
// other functions as ThisPtr (or Source). Use ThisPtr (or Source) to access
// instance specific data.
// In this example, these parameters will be of type TInt32ConverterInstance*.


typedef struct TInt32ConverterInstance {
    // NOTE: for simpliciy the array sizes were chosen statically for the
    // int32_t type, but you may need to dynamically allocate memory for other
    // converter types to not waste memory; in this case, make sure to allocate
    // and initialize in CreateConverter, and deallocate in DestroyConverter.
    // What matters is that each converter instance has its own memory, that
    // holds converted data returned by BytesToStr and StrToBytes (see the
    // comment at file start).
    wchar_t ConvertedStr[128]; // smaller should be fine, but depends on maximal length of formatted string
    uint8_t ConvertedBytes[sizeof(int32_t)];
} TInt32ConverterInstance;

void* __stdcall CreateConverter(TConverterClassID ClassIDOrFactoryFunc,
    const wchar_t** TypeName, const wchar_t** FriendlyTypeName,
    TDataTypeWidth* Width, int* MaxTypeSize, TByteOrders* SupportedByteOrders,
    BOOL* SupportsStrToBytes)
{
    // ClassIDOrFactoryFunc can be used to delegate creation to constructor
    // functions as needed. See the C++ plugin for an example.

    // Here we just support one converter type, so it suffices to check we were
    // called to construct the right converter.
    assert(ClassIDOrFactoryFunc == &Int32ConverterClassID);

    *TypeName = L"C - Int32";
    *FriendlyTypeName = *TypeName;
    *Width = dtwFixed;
    *MaxTypeSize = sizeof(int32_t);
    *SupportedByteOrders = 1 << boLittleEndian | 1 << boBigEndian;
    *SupportsStrToBytes = TRUE;

    // create instance specific storage
    TInt32ConverterInstance* Converter = malloc(sizeof(TInt32ConverterInstance));
    memset(Converter, 0, sizeof(TInt32ConverterInstance));
    return Converter;
}

void __stdcall DestroyConverter(void* ThisPtr)
{
    // free instance specific storage
    free(ThisPtr);
}

void __stdcall AssignConverter(void* ThisPtr, void* Source)
{
    memcpy(((TInt32ConverterInstance*)ThisPtr)->ConvertedBytes,
        ((TInt32ConverterInstance*)Source)->ConvertedBytes,
        FIELD_SIZEOF(TInt32ConverterInstance, ConvertedBytes));

    memcpy(((TInt32ConverterInstance*)ThisPtr)->ConvertedStr,
        ((TInt32ConverterInstance*)Source)->ConvertedStr,
        FIELD_SIZEOF(TInt32ConverterInstance, ConvertedStr));
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
    TInt32ConverterInstance* Converter = ((TInt32ConverterInstance*)ThisPtr);

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

        swprintf(Converter->ConvertedStr, ARRAY_LENGTH(Converter->ConvertedStr),
            FormatStr, *(int32_t*)(Bytes));
        *ConvertedStr = Converter->ConvertedStr;

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

    TInt32ConverterInstance* Converter = ((TInt32ConverterInstance*)ThisPtr);

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
    
    // hexadecimal numbers are always handled as unsigned integers
    if (result == stbeOverflow)
    {
        unsigned long UI;
        result = str2uint(trimmed_str, &UI, base);
        I = (long)UI;
    }
    
    free(cloned_str);
    
    *((int32_t*)Converter->ConvertedBytes) = (int32_t)I;
    *ConvertedBytes = Converter->ConvertedBytes;
    
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


// Actual value does not matter (so assign 0), we only need the address to this
// constant storage, see RegisterDataTypeConverter() call in dllmain.c.
//
// The address will serve as class ID; it will be unique, since plugins / DLLs
// and the main program, HxD, are in the same process / address space.
//
// Other languages than C can dynamically allocate a variable when the DLL gets
// loaded, or actually reference an object or a function that is specific to
// that type converter. What matters is that the pointer is used to uniquely
// identify the converter type, such that it can be used in CreateConverter()
// for the parameter ClassIDOrFactoryFunc.
volatile const int Int32ConverterClassID = 0;
