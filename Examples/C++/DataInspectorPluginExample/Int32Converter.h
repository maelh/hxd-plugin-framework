#pragma once

#include "DataInspectorShared.h"
#include "DataInspectorPluginServer.h"

class TInt32Converter : public TExternalDataTypeConverter
{
public:
    // Class factory function / virtual constructor
    static TExternalDataTypeConverter* Create();
public:
    TInt32Converter();

    void ChangeByteOrder(uint8_t* Bytes, int ByteCount,
        TByteOrder TargetByteOrder) override;
    TBytesToStrError BytesToStr(uint8_t* Bytes, int ByteCount,
        TFormattingOptions FormattingOptions, int& ConvertedByteCount,
        std::wstring& ConvertedStr) override;
    TStrToBytesError StrToBytes(std::wstring Str,
        TFormattingOptions FormattingOptions,
        std::vector<uint8_t>& ConvertedBytes) override;

    TBytesToIntError AsInt64(uint8_t* Bytes, int ByteCount,
        int& ConvertedByteCount, int64_t& ConvertedInt) override;
    TBytesToIntError AsUInt64(uint8_t* Bytes, int ByteCount,
        int& ConvertedByteCount, uint64_t& ConvertedInt) override;
};
