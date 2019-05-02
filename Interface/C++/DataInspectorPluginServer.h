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
		TIntegerDisplayOption IntegerDisplayOption, int& ConvertedBytesCount,
		std::wstring& ConvertedStr) = 0;
	virtual TStrToBytesError StrToBytes(std::wstring Str,
		TIntegerDisplayOption IntegerDisplayOption,
		std::vector<uint8_t>& ConvertedBytes) = 0;

protected:
	std::wstring FName;
	TDataTypeWidth FWidth;
	int FMaxTypeSize;
	TByteOrders FSupportedByteOrders;

// Getters
public:
	const std::wstring& GetName() { return FName; }
	const TDataTypeWidth& GetWidth() { return FWidth; }
	const int& GetMaxTypeSize() { return FMaxTypeSize; }
	const TByteOrders& GetSupportedByteOrders() { return FSupportedByteOrders; }
};

// Class factory function / virtual constructor
typedef TExternalDataTypeConverter* (*TExternalDataTypeConverterFactoryFunction)();

BOOL __stdcall GetDataTypeConverters(
	PDataTypeConverterPluginInterface* ConvInterfaces, int* ConvInterfaceCount);

void RegisterDataTypeConverter(
	TExternalDataTypeConverterFactoryFunction FactoryFunction);
