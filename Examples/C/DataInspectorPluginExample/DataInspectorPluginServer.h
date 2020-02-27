#pragma once

#include "DataInspectorShared.h"
#include "DataInspectorPluginInterface.h"

// Class factory function / virtual constructor
typedef void* (*TExternalDataTypeConverterFactoryFunction)();

void RegisterDataTypeConverter(
    TExternalDataTypeConverterFactoryFunction ConverterFactoryFunc);
