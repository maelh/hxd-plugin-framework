#pragma once

#include "Windows.h"
#include "DataInspectorShared.h"

typedef void* TConverterType;
typedef TConverterType* PConverterType;

typedef void* (__stdcall *TDTCPCreateConverter) (
	TConverterType ConvType, 
	const wchar_t** Name,
	TDataTypeWidth* Width, 
	int* MaxTypeSize,
	TByteOrders* SupportedByteOrders
);

typedef void (__stdcall *TDTCPDeleteConverter) (
	void* Converter
);

typedef void (__stdcall *TDTCPAssignConverter) (
	void* ThisPtr,
	void* Source
);

typedef void (__stdcall *TDTCPChangeByteOrder) (
	void* ThisPtr,
	uint8_t* Bytes,
	int ByteCount,
	TByteOrder TargetByteOrder
);

typedef TBytesToStrError (__stdcall *TDTCPBytesToStr) (
	void* ThisPtr,
	uint8_t* Bytes,
	int ByteCount,
	TIntegerDisplayOption IntegerDisplayOption,
	int* ConvertedByteCount,
	const wchar_t** ConvertedStr
);

typedef TStrToBytesError (__stdcall *TDTCPStrToBytes) (
	void* ThisPtr,
	const wchar_t* Str,
	TIntegerDisplayOption IntegerDisplayOption,
	uint8_t** ConvertedBytes,
	int* ConvertedByteCount
);

typedef struct TDataTypeConverterPluginInterface {
	TConverterType ConverterType;

	TDTCPCreateConverter CreateConverter;
	TDTCPDeleteConverter DeleteConverter;
	TDTCPAssignConverter AssignConverter;

	TDTCPChangeByteOrder ChangeByteOrder;
	TDTCPBytesToStr BytesToStr;
	TDTCPStrToBytes	StrToBytes;
} TDataTypeConverterPluginInterface;

typedef TDataTypeConverterPluginInterface* PDataTypeConverterPluginInterface;

typedef BOOL (__stdcall *TGetDataTypeConverters) (
	PDataTypeConverterPluginInterface* ConvInterfaces,
	int* ConvInterfaceCount
);
