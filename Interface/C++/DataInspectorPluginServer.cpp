#include "DataInspectorPluginServer.h"

TExternalDataTypeConverter::TExternalDataTypeConverter()
{
}

void TExternalDataTypeConverter::Assign(TExternalDataTypeConverter* Source)
{
	FName = Source->FName;
	FWidth = Source->FWidth;
	FMaxTypeSize = Source->FMaxTypeSize;
	FSupportedByteOrders = Source->FSupportedByteOrders;
}


class TRawToClassAdapter
{
private:
	static std::wstring FLastReturnedString;
	static std::vector<uint8_t> FLastReturnedByteArray;
public:
	static void* __stdcall CreateConverter(TConverterType ConvType,
		const wchar_t** Name, TDataTypeWidth* Width, int* MaxTypeSize,
		TByteOrders* SupportedByteOrders);

	static void __stdcall DeleteConverter(void* Converter);

	static void __stdcall AssignConverter(void* ThisPtr, void* Source);

	static void __stdcall ChangeByteOrder(void* ThisPtr, uint8_t* Bytes,
		int ByteCount, TByteOrder TargetByteOrder);

	static TBytesToStrError __stdcall BytesToStr(void* ThisPtr, uint8_t* Bytes,
		int ByteCount, TIntegerDisplayOption IntegerDisplayOption,
		int* ConvertedBytesCount, const wchar_t** ConvertedStr);

	static TStrToBytesError __stdcall StrToBytes(void* ThisPtr,
		const wchar_t* Str,	TIntegerDisplayOption IntegerDisplayOption,
		uint8_t** ConvertedBytes, int* ConvertedByteCount);
};

std::wstring TRawToClassAdapter::FLastReturnedString;
std::vector<uint8_t> TRawToClassAdapter::FLastReturnedByteArray;

void* TRawToClassAdapter::CreateConverter(TConverterType ConvType,
	const wchar_t** Name, TDataTypeWidth* Width, int* MaxTypeSize,
	TByteOrders* SupportedByteOrders)
{
	TExternalDataTypeConverterFactoryFunction FactoryFunction =
		(TExternalDataTypeConverterFactoryFunction)ConvType;

	TExternalDataTypeConverter* Converter = FactoryFunction();

	*Name = Converter->GetName().c_str();
	*Width = Converter->GetWidth();
	*MaxTypeSize = Converter->GetMaxTypeSize();
	*SupportedByteOrders = Converter->GetSupportedByteOrders();

	return Converter;
}

void TRawToClassAdapter::DeleteConverter(void* Converter)
{
	delete ((TExternalDataTypeConverter*)Converter);
}

void TRawToClassAdapter::AssignConverter(void* ThisPtr, void* Source)
{
	((TExternalDataTypeConverter*)ThisPtr)->Assign(
		(TExternalDataTypeConverter*)Source);
}

void TRawToClassAdapter::ChangeByteOrder(void* ThisPtr, uint8_t* Bytes,
	int ByteCount, TByteOrder TargetByteOrder)
{
	((TExternalDataTypeConverter*)ThisPtr)->ChangeByteOrder(Bytes, ByteCount,
		TargetByteOrder);
}

TBytesToStrError TRawToClassAdapter::BytesToStr(void* ThisPtr, uint8_t* Bytes,
	int ByteCount, TIntegerDisplayOption IntegerDisplayOption,
	int* ConvertedBytesCount, const wchar_t** ConvertedStr)
{
	FLastReturnedString.clear();

	TBytesToStrError result = ((TExternalDataTypeConverter*)ThisPtr)->
		BytesToStr(
			Bytes, ByteCount, IntegerDisplayOption, *ConvertedBytesCount,
			FLastReturnedString);
	
	*ConvertedStr = FLastReturnedString.c_str();

	return result;
}

TStrToBytesError TRawToClassAdapter::StrToBytes(void* ThisPtr,
	const wchar_t* Str,	TIntegerDisplayOption IntegerDisplayOption,
	uint8_t** ConvertedBytes, int* ConvertedByteCount)
{
	FLastReturnedByteArray.clear();
	
	TStrToBytesError result = ((TExternalDataTypeConverter*)ThisPtr)->
		StrToBytes(
			Str, IntegerDisplayOption, FLastReturnedByteArray);

	if (FLastReturnedByteArray.size() > 0)
		*ConvertedBytes = &FLastReturnedByteArray[0];
	else
		*ConvertedBytes = NULL;
	*ConvertedByteCount = (int)FLastReturnedByteArray.size();

	return result;
}

std::vector<TDataTypeConverterPluginInterface> ConverterInterfaces;

BOOL __stdcall GetDataTypeConverters(
	PDataTypeConverterPluginInterface* ConvInterfaces, int* ConvInterfaceCount)
{
	if (ConverterInterfaces.size() > 0)
		*ConvInterfaces = &ConverterInterfaces[0];
	else
		*ConvInterfaces = NULL;
	*ConvInterfaceCount = (int)ConverterInterfaces.size();

	return TRUE;
}

void RegisterDataTypeConverter(
	TExternalDataTypeConverterFactoryFunction FactoryFunction)
{
	TDataTypeConverterPluginInterface ConvIntf = {
		FactoryFunction,
		TRawToClassAdapter::CreateConverter,
		TRawToClassAdapter::DeleteConverter,
		TRawToClassAdapter::AssignConverter,
		TRawToClassAdapter::ChangeByteOrder,
		TRawToClassAdapter::BytesToStr,
		TRawToClassAdapter::StrToBytes
	};

	ConverterInterfaces.push_back(ConvIntf);
}
