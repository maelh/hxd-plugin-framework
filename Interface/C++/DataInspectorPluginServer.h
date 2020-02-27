#pragma once

#include <string>
#include <vector>

#include "DataInspectorShared.h"
#include "DataInspectorPluginInterface.h"

class TExternalDataTypeConverter
{
public:
    TExternalDataTypeConverter();

    virtual void Assign(TExternalDataTypeConverter* Source);

    virtual void ChangeByteOrder(uint8_t* Bytes, int ByteCount,
        TByteOrder TargetByteOrder) = 0;
    virtual TBytesToStrError BytesToStr(uint8_t* Bytes, int ByteCount,
        TIntegerDisplayOption IntegerDisplayOption, int& ConvertedByteCount,
        std::wstring& ConvertedStr) = 0;
    virtual TStrToBytesError StrToBytes(std::wstring Str,
        TIntegerDisplayOption IntegerDisplayOption,
        std::vector<uint8_t>& ConvertedBytes) = 0;

protected:
    std::wstring FTypeName;
    std::wstring FFriendlyTypeName;
    TDataTypeWidth FWidth;
    int FMaxTypeSize;
    TByteOrders FSupportedByteOrders;

private:
    std::wstring FLastReturnedString;
    std::vector<uint8_t> FLastReturnedByteArray;
    friend TBytesToStrError __stdcall BytesToStr(void*, uint8_t*, int,
        TIntegerDisplayOption, int*, const wchar_t**);
    friend TStrToBytesError __stdcall StrToBytes(void*, const wchar_t*,
        TIntegerDisplayOption, uint8_t**, int*);

// Getters
public:
    const std::wstring& GetTypeName() { return FTypeName; }
    const std::wstring& GetFriendlyTypeName() { return FFriendlyTypeName; }
    const TDataTypeWidth& GetWidth() { return FWidth; }
    const int& GetMaxTypeSize() { return FMaxTypeSize; }
    const TByteOrders& GetSupportedByteOrders() { return FSupportedByteOrders; }
};



// Class factory function / virtual constructor
typedef TExternalDataTypeConverter* (*TExternalDataTypeConverterFactoryFunction)();

void RegisterDataTypeConverter(
    TExternalDataTypeConverterFactoryFunction ConverterFactoryFunc);
