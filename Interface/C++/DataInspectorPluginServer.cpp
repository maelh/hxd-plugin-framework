#include "DataInspectorPluginServer.h"

TExternalDataTypeConverter::TExternalDataTypeConverter()
{
}

void TExternalDataTypeConverter::Assign(TExternalDataTypeConverter* Source)
{
    FTypeName = Source->FTypeName;
    FFriendlyTypeName = Source->FFriendlyTypeName;
    FWidth = Source->FWidth;
    FMaxTypeSize = Source->FMaxTypeSize;
    FSupportedByteOrders = Source->FSupportedByteOrders;

    FLastReturnedString = Source->FLastReturnedString;
    FLastReturnedByteArray = Source->FLastReturnedByteArray;
}

void* __stdcall CreateConverter(TConverterClassID ClassIDOrFactoryFunc,
    const wchar_t** TypeName, const wchar_t** FriendlyTypeName,
    TDataTypeWidth* Width, int* MaxTypeSize, TByteOrders* SupportedByteOrders)
{
    TExternalDataTypeConverterFactoryFunction FactoryFunc =
        (TExternalDataTypeConverterFactoryFunction)ClassIDOrFactoryFunc;

    TExternalDataTypeConverter* Converter = FactoryFunc();

    *TypeName = Converter->GetTypeName().c_str();
    *FriendlyTypeName = Converter->GetFriendlyTypeName().c_str();
    *Width = Converter->GetWidth();
    *MaxTypeSize = Converter->GetMaxTypeSize();
    *SupportedByteOrders = Converter->GetSupportedByteOrders();

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
    int ByteCount, TIntegerDisplayOption IntegerDisplayOption,
    int* ConvertedByteCount, const wchar_t** ConvertedStr)
{
    TExternalDataTypeConverter* Converter = (TExternalDataTypeConverter*)ThisPtr;

    Converter->FLastReturnedString.clear();

    TBytesToStrError result = Converter->BytesToStr(Bytes, ByteCount,
        IntegerDisplayOption, *ConvertedByteCount,
        Converter->FLastReturnedString);

    *ConvertedStr = Converter->FLastReturnedString.c_str();

    return result;
}

TStrToBytesError __stdcall StrToBytes(void* ThisPtr, const wchar_t* Str,
    TIntegerDisplayOption IntegerDisplayOption, uint8_t** ConvertedBytes,
    int* ConvertedByteCount)
{
    TExternalDataTypeConverter* Converter = (TExternalDataTypeConverter*)ThisPtr;

    Converter->FLastReturnedByteArray.clear();

    TStrToBytesError result = Converter->StrToBytes(Str, IntegerDisplayOption,
        Converter->FLastReturnedByteArray);

    if (Converter->FLastReturnedByteArray.size() > 0)
        *ConvertedBytes = &Converter->FLastReturnedByteArray[0];
    else
        *ConvertedBytes = NULL;
    *ConvertedByteCount = (int)Converter->FLastReturnedByteArray.size();

    return result;
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
