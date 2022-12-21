#include "hidedriver.h"
#include "communication.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	pDriverObject->DriverUnload = UnloadDriver;
	DebugMessage("Welcome to the first HIDE DRIVER!\n");

	//DWORD a = 3300;
	//HideTask(&a);

	RtlInitUnicodeString(&dev, L"\\Device\\hidedriver");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\hidedriver");

	IoCreateDevice(pDriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&dos, &dev);

	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriverObject->MajorFunction[i] = defaultDispatch;
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;

	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;

	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriveObject)
{
	DebugMessage("HIDE DRIVER SAYS GOODBYE!\n");

	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(pDriveObject->DeviceObject);

	return STATUS_SUCCESS;
}