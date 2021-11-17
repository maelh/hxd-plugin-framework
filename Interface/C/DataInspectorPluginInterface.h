#pragma once

#ifdef __cplusplus  
extern "C" {
#endif

#include "Windows.h"
#include "DataInspectorShared.h"

BOOL __stdcall GetDataTypeConverterClassIDs(
    PConverterClassID* ClassIDsOrFactoryFuncs,
    int* Count);


void* __stdcall CreateConverter(
    TConverterClassID ClassIDOrFactoryFunc,
    const wchar_t** TypeName,
    const wchar_t** FriendlyTypeName,
    TTypeCategory* Category,
    TDataTypeWidth* Width,
    int* MaxTypeSize,
    TByteOrders* SupportedByteOrders,
    BOOL* SupportsStrToBytes);

void __stdcall DestroyConverter(
    void* ThisPtr);

void __stdcall AssignConverter(
    void* ThisPtr,
    void* Source);


void __stdcall ChangeByteOrder(
    void* ThisPtr,
    uint8_t* Bytes,
    int ByteCount,
    TByteOrder TargetByteOrder);

TBytesToStrError __stdcall BytesToStr(
    void* ThisPtr,
    uint8_t* Bytes,
    int ByteCount,
    TFormattingOptions FormattingOptions,
    int* ConvertedByteCount,
    const wchar_t** ConvertedStr);

TStrToBytesError __stdcall StrToBytes(
    void* ThisPtr,
    const wchar_t* Str,
    TFormattingOptions FormattingOptions,
    uint8_t** ConvertedBytes,
    int* ConvertedByteCount);

TBytesToIntError __stdcall AsInt64(
    void* ThisPtr,
    uint8_t* Bytes,
    int ByteCount,
    int* ConvertedByteCount,
    int64_t* ConvertedInt);

TBytesToIntError __stdcall AsUInt64(
    void* ThisPtr,
    uint8_t* Bytes,
    int ByteCount,
    int* ConvertedByteCount,
    uint64_t* ConvertedInt);

#ifdef __cplusplus  
}
#endif
