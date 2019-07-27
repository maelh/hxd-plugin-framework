#include "DataInspectorPluginServer.h"

// NOTE: a more flexible implementation would use a kind of dynamic array, such
// as provided by many languages by default. Implementing a dynamic array in C
// would just add clutter, and distract from giving a simple example; so we
// chose an arbitrary limit of 10.
#define MaxConvTypes 10

TConverterType ConvTypes[MaxConvTypes];
int ConvTypeCount = 0;

BOOL __stdcall GetDataTypeConverters(
    PDataTypeConverterPluginInterface* ConvInterfaces, int* ConvInterfaceCount)
{
    *ConvInterfaceCount = ConvTypeCount;
    *ConvInterfaces = ConvTypes[0];

    return TRUE;
}

void RegisterDataTypeConverter(TConverterType ConvType)                         
{
    if (ConvTypeCount < MaxConvTypes)
    {
        ConvTypes[ConvTypeCount] = ConvType;
        ConvTypeCount++;
    }
}
