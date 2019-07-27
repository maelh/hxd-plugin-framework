#include "Windows.h"
#include "DataInspectorPluginServer.h"
#include "DataInspectorPluginExample.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
    PVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        RegisterDataTypeConverter(&ExampleDTCClass);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
