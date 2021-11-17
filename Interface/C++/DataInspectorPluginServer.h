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
        TFormattingOptions FormattingOptions, int& ConvertedByteCount,
        std::wstring& ConvertedStr) = 0;
    virtual TStrToBytesError StrToBytes(std::wstring Str,
        TFormattingOptions FormattingOptions,
        std::vector<uint8_t>& ConvertedBytes) = 0;

    virtual TBytesToIntError AsInt64(uint8_t* Bytes, int ByteCount,
        int& ConvertedByteCount, int64_t& ConvertedInt) = 0;
    virtual TBytesToIntError AsUInt64(uint8_t* Bytes, int ByteCount,
        int& ConvertedByteCount, uint64_t& ConvertedInt) = 0;

protected:
    std::wstring FTypeName;
    std::wstring FFriendlyTypeName;
    TTypeCategory FCategory;
    TDataTypeWidth FWidth;
    int FMaxTypeSize;
    TByteOrders FSupportedByteOrders;
    BOOL FSupportsStrToBytes;

private:
    std::wstring FLastReturnedString;
    std::vector<uint8_t> FLastReturnedByteArray;
    friend TBytesToStrError __stdcall BytesToStr(void*, uint8_t*, int,
        TFormattingOptions, int*, const wchar_t**);
    friend TStrToBytesError __stdcall StrToBytes(void*, const wchar_t*,
        TFormattingOptions, uint8_t**, int*);

// Getters
public:
    const std::wstring& GetTypeName() { return FTypeName; }
    const std::wstring& GetFriendlyTypeName() { return FFriendlyTypeName; }
    const TTypeCategory& GetCategory() { return FCategory; }
    const TDataTypeWidth& GetWidth() { return FWidth; }
    const int& GetMaxTypeSize() { return FMaxTypeSize; }
    const TByteOrders& GetSupportedByteOrders() { return FSupportedByteOrders; }
    const BOOL GetSupportsStrToBytes() { return FSupportsStrToBytes; }
};



// Class factory function / virtual constructor
typedef TExternalDataTypeConverter* (*TExternalDataTypeConverterFactoryFunction)();

void RegisterDataTypeConverter(
    TExternalDataTypeConverterFactoryFunction ConverterFactoryFunc);
