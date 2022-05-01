

#include <windows.h>         // HANDLE
#include "type.h"

EXPORT_DLL int add(int a, int b);
EXPORT_DLL void close_handle(HANDLE handle);
EXPORT_DLL BYTE get_bus_type(HANDLE handle);
EXPORT_DLL BYTE scsi_pass_through_direct(HANDLE handle, BYTE* cdb, BYTE* buffer, ULONG dataXferLen, BYTE direction, WORD timeout);
EXPORT_DLL HANDLE open_device(echar_p devicePath);