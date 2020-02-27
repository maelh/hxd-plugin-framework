#pragma once

#include "DataInspectorShared.h"
#include "DataInspectorPluginInterface.h"

// Class factory function / virtual constructor or unique class ID
typedef void* (*TClassIDOrFactoryFunc)();

void RegisterDataTypeConverter(TClassIDOrFactoryFunc ClassIDOrFactoryFunc);
