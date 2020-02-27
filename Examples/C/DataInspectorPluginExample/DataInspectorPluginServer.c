#include "DataInspectorPluginServer.h"

// NOTE: a more flexible implementation would use a kind of dynamic array, such
// as provided by many languages by default. Implementing a dynamic array in C
// would just add clutter, and distract from giving a simple example; so we
// chose an arbitrary limit of 10.
#define MaxConverterCount 10

TConverterClassID InternalClassIDsOrFactoryFuncs[MaxConverterCount];
int ConverterCount = 0;


void RegisterDataTypeConverter(
    TExternalDataTypeConverterFactoryFunction ConverterFactoryFunc)
{
    if (ConverterCount < MaxConverterCount)
    {
        InternalClassIDsOrFactoryFuncs[ConverterCount] = ConverterFactoryFunc;
        ConverterCount++;
    }
}

BOOL __stdcall GetDataTypeConverterClassIDs(
    PConverterClassID* ClassIDsOrFactoryFuncs, int* Count)
{
    if (ConverterCount > 0)
        *ClassIDsOrFactoryFuncs = &InternalClassIDsOrFactoryFuncs[0];
    else
        *ClassIDsOrFactoryFuncs = NULL;
    *Count = ConverterCount;

    return TRUE;
}
