#pragma once
#pragma warning (disable : 4100)

#include <ntifs.h>
#include "datatype.h"
#include "data.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriveObject);

