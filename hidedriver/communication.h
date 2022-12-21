#pragma once

#include <ntifs.h>
#include "hide.h"
#include "dbgmsg.h"

#define IO_HIDE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x666, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

NTSTATUS IoControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp);

NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp);

NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp);

NTSTATUS defaultDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIRP);