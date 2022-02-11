#include "DataInspectorPluginServer.h"

TExternalDataTypeConverter::TExternalDataTypeConverter()
{
}

void TExternalDataTypeConverter::Assign(TExternalDataTypeConverter* Source)
{
    FTypeName = Source->FTypeName;
    FFriendlyTypeName = Source->FFriendlyTypeName;
    FCategory = Source->FCategory;
    FWidth = Source->FWidth;
    FMaxTypeSize = Source->FMaxTypeSize;
    FSupportedByteOrders = Source->FSupportedByteOrders;
    FSupportsStrToBytes = Source->FSupportsStrToBytes;

    FLastReturnedString = Source->FLastReturnedString;
    FLastReturnedByteArray = Source->FLastReturnedByteArray;
}

void* __stdcall CreateConverter(TConverterClassID ClassIDOrFactoryFunc,
    const wchar_t** TypeName, const wchar_t** FriendlyTypeName,
    TTypeCategory* Category, TDataTypeWidth* Width, int* MaxTypeSize,
    TByteOrders* SupportedByteOrders, int* Flags)
{
    TExternalDataTypeConverterFactoryFunction FactoryFunc =
        (TExternalDataTypeConverterFactoryFunction)ClassIDOrFactoryFunc;

    TExternalDataTypeConverter* Converter = FactoryFunc();

    *TypeName = Converter->GetTypeName().c_str();
    *FriendlyTypeName = Converter->GetFriendlyTypeName().c_str();
    *Category = Converter->GetCategory();
    *Width = Converter->GetWidth();
    *MaxTypeSize = Converter->GetMaxTypeSize();
    *SupportedByteOrders = Converter->GetSupportedByteOrders();
    *Flags = Converter->GetSupportsStrToBytes() ? ccfSupportsStrToBytes : 0;

    return Converter;
}

void __stdcall DestroyConverter(void* ThisPtr)
{
    delete ((TExternalDataTypeConverter*)ThisPtr);
}

void __stdcall AssignConverter(void* ThisPtr, void* Source)
{
    ((TExternalDataTypeConverter*)ThisPtr)->Assign(
        (TExternalDataTypeConverter*)Source);
}

void __stdcall ChangeByteOrder(void* ThisPtr, uint8_t* Bytes, int ByteCount,
    TByteOrder TargetByteOrder)
{
    ((TExternalDataTypeConverter*)ThisPtr)->ChangeByteOrder(Bytes, ByteCount,
        TargetByteOrder);
}

TBytesToStrError __stdcall BytesToStr(void* ThisPtr, uint8_t* Bytes,
    int ByteCount, TFormattingOptions FormattingOptions,
    int* ConvertedByteCount, const wchar_t** ConvertedStr)
{
    TExternalDataTypeConverter* Converter = (TExternalDataTypeConverter*)ThisPtr;

    Converter->FLastReturnedString.clear();

    TBytesToStrError result = Converter->BytesToStr(Bytes, ByteCount,
        FormattingOptions, *ConvertedByteCount, Converter->FLastReturnedString);

    *ConvertedStr = Converter->FLastReturnedString.c_str();

    return result;
}

TStrToBytesError __stdcall StrToBytes(void* ThisPtr, const wchar_t* Str,
    TFormattingOptions FormattingOptions, uint8_t** ConvertedBytes,
    int* ConvertedByteCount)
{
    TExternalDataTypeConverter* Converter = (TExternalDataTypeConverter*)ThisPtr;

    Converter->FLastReturnedByteArray.clear();

    TStrToBytesError result = Converter->StrToBytes(Str, FormattingOptions,
        Converter->FLastReturnedByteArray);

    if (Converter->FLastReturnedByteArray.size() > 0)
        *ConvertedBytes = &Converter->FLastReturnedByteArray[0];
    else
        *ConvertedBytes = NULL;
    *ConvertedByteCount = (int)Converter->FLastReturnedByteArray.size();

    return result;
}

TBytesToIntError __stdcall AsInt64(void* ThisPtr, uint8_t* Bytes,
    int ByteCount, int* ConvertedByteCount, int64_t* ConvertedInt)
{
    TExternalDataTypeConverter* Converter = (TExternalDataTypeConverter*)ThisPtr;

    return Converter->AsInt64(Bytes, ByteCount, *ConvertedByteCount,
        *ConvertedInt);
}

TBytesToIntError __stdcall AsUInt64(void* ThisPtr, uint8_t* Bytes,
    int ByteCount, int* ConvertedByteCount, uint64_t* ConvertedInt)
{
    TExternalDataTypeConverter* Converter = (TExternalDataTypeConverter*)ThisPtr;

    return Converter->AsUInt64(Bytes, ByteCount, *ConvertedByteCount,
        *ConvertedInt);
}

std::vector<TConverterClassID> InternalClassIDsOrFactoryFuncs;

void RegisterDataTypeConverter(
    TExternalDataTypeConverterFactoryFunction ConverterFactoryFunc)
{
    InternalClassIDsOrFactoryFuncs.push_back(ConverterFactoryFunc);
}

BOOL __stdcall GetDataTypeConverterClassIDs(
    PConverterClassID* ClassIDsOrFactoryFuncs, int* Count)
{
    if (InternalClassIDsOrFactoryFuncs.size() > 0)
        *ClassIDsOrFactoryFuncs = &InternalClassIDsOrFactoryFuncs[0];
    else
        *ClassIDsOrFactoryFuncs = NULL;
    *Count = (int)InternalClassIDsOrFactoryFuncs.size();

    return TRUE;
}
