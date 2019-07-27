#pragma once

#include "DataInspectorShared.h"
#include "DataInspectorPluginInterface.h"

BOOL __stdcall GetDataTypeConverters(
	PDataTypeConverterPluginInterface* ConvInterfaces, int* ConvInterfaceCount);

void RegisterDataTypeConverter(TConverterType ConvType);
